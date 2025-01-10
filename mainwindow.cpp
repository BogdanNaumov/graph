#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QInputDialog>
#include <QMessageBox>
#include <QCheckBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Связываем пункт меню "Открыть" с обработчиком
    connect(ui->actionOpenFile, &QAction::triggered, this, &MainWindow::openFile);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFile()
{
    // Выбор файла
    QString filePath = QFileDialog::getOpenFileName(this, tr("Открыть файл"), "", tr("TAB Files (*.tab);;All Files (*)"));
    if (filePath.isEmpty())
        return;

    // Ввод количества строк для парсинга
    bool ok;
    int lineCount = QInputDialog::getInt(this, tr("Количество строк"), tr("Сколько строк парсить?"), 10, 1, 1000000, 1, &ok);
    if (!ok)
        return;

    // Открываем файл
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось открыть файл"));
        return;
    }

    QTextStream in(&file);
    QStringList variableNames;

    // Читаем первую строку и извлекаем заголовки
    if (!in.atEnd()) {
        QString headerLine = in.readLine();
        variableNames = headerLine.split('\t', Qt::SkipEmptyParts);
        if (!variableNames.isEmpty() && variableNames.first() == "time") {
            variableNames.removeFirst(); // Убираем "time" из списка переменных
        }
    }

    // Читаем данные (но в этой версии только заголовки нужны)
    int parsedLines = 0;
    while (!in.atEnd() && parsedLines < lineCount) {
        QString line = in.readLine();
        parsedLines++;
    }

    file.close();

    // Обновляем переменные в области справа
    updateVariables(variableNames);
}

void MainWindow::updateVariables(const QStringList &variables)
{
    // Очистка предыдущих переменных
    QLayout *layout = ui->variablesLayout;
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // Добавляем чекбоксы для каждой переменной
    for (const QString &var : variables) {
        QCheckBox *checkBox = new QCheckBox(var, this);
        layout->addWidget(checkBox);

        // Пример: подключение сигнала на отображение переменной
        connect(checkBox, &QCheckBox::toggled, this, [this, var](bool checked) {
            toggleVariableOnGraph(var, checked);
        });
    }
}

void MainWindow::toggleVariableOnGraph(const QString &variable, bool visible)
{
    // TODO: Реализовать логику отображения/скрытия переменной на графике
    // Например, добавить/удалить соответствующий график из QCustomPlot
}
