#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QInputDialog>
#include <QMessageBox>
#include <QCheckBox>
#include <QVector>
#include <QMap>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Инициализация QCustomPlot
    customPlot = new QCustomPlot(this);
    ui->plotArea->setLayout(new QVBoxLayout);
    ui->plotArea->layout()->addWidget(customPlot);

    // Связываем пункт меню "Открыть" с обработчиком
    connect(ui->actionOpenFile, &QAction::triggered, this, &MainWindow::openFile);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFile()
{
    // Открываем диалог выбора файла
    QString filePath = QFileDialog::getOpenFileName(this, tr("Открыть файл"), "", tr("TAB Files (*.tab);;All Files (*)"));
    if (filePath.isEmpty())
        return;

    // Запрашиваем количество строк для парсинга
    bool ok;
    int lineCount = QInputDialog::getInt(this, tr("Количество строк"), tr("Сколько строк парсить?"), 10, 1, 1000000, 1, &ok);
    if (!ok)
        return;

    // Открываем файл
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось открыть файл"));
        return;
    }

    QTextStream in(&file);
    QStringList variableNames;
    QVector<double> timeData; // Глобальное время
    QMap<QString, QVector<double>> variablesData;

    // Чтение заголовков
    if (!in.atEnd())
    {
        QString headerLine = in.readLine();
        variableNames = headerLine.split('\t', Qt::SkipEmptyParts);

        qDebug() << "Header line:" << headerLine;
        qDebug() << "Variable names:" << variableNames;

        if (!variableNames.isEmpty() && variableNames.first() == "time") {
            variableNames.removeFirst(); // Убираем "time", если он присутствует
        }
    }

    int parsedLines = 0;
    double globalTime = 0.0;

    // Чтение строк файла
    while (!in.atEnd() && parsedLines < lineCount)
    {
        QString line = in.readLine();
        QStringList values = line.split('\t', Qt::SkipEmptyParts);

        qDebug() << "Parsed line:" << line;
        qDebug() << "Split values:" << values;

        if (values.size() != variableNames.size() + 1) {
            qDebug() << "Skipping line due to mismatch between header and row sizes.";
            continue;
        }

        bool ok = false;
        double dtValue = values[0].toDouble(&ok);
        if (!ok) {
            qDebug() << "Skipping line due to invalid dt value:" << values[0];
            continue;
        }

        // Проверяем, что dtValue действительно 0.0025
        if (qFabs(dtValue - 0.0025) > 1e-6) {
            qDebug() << "Unexpected dtValue:" << dtValue;
        }

        globalTime += dtValue;
        timeData.append(globalTime);

        for (int i = 0; i < variableNames.size(); ++i) {
            bool valueOk = false;
            double varValue = values[i + 1].toDouble(&valueOk);
            if (valueOk) {
                variablesData[variableNames[i]].append(varValue);
            } else {
                qDebug() << "Invalid value for variable:" << variableNames[i] << ", at line:" << parsedLines;
            }
        }

        parsedLines++;
    }


    qDebug() << "Final timeData:" << timeData;
    for (const QString &key : variablesData.keys()) {
        qDebug() << "Variable:" << key << ", Data:" << variablesData[key];
    }

    file.close();

    // Проверка данных
    qDebug() << "timeData:" << timeData;
    for (const QString &key : variablesData.keys())
    {
        qDebug() << "Variable:" << key << ", Data:" << variablesData[key];
    }

    // Обновление интерфейса
    updateVariables(variableNames);

    this->timeData = timeData;
    this->variablesData = variablesData;

    redrawGraph();
}

void MainWindow::updateVariables(const QStringList &variables)
{
    // Очистка списка переменных
    QLayout *layout = ui->variablesLayout;
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr)
    {
        delete item->widget();
        delete item;
    }

    // Добавление чекбоксов
    for (const QString &var : variables)
    {
        QCheckBox *checkBox = new QCheckBox(var, this);
        layout->addWidget(checkBox);

        connect(checkBox, &QCheckBox::toggled, this, [this, var](bool checked)
                { toggleVariableOnGraph(var, checked); });
    }
}

void MainWindow::toggleVariableOnGraph(const QString &variable, bool visible)
{
    if (visible)
    {
        if (timeData.size() != variablesData[variable].size())
        {
            qDebug() << "Размеры timeData и" << variable << "не совпадают!";
            return;
        }

        QCPGraph *graph = customPlot->addGraph();
        graph->setData(timeData, variablesData[variable]);
        graph->setName(variable);
    }
    else
    {
        for (int i = 0; i < customPlot->graphCount(); ++i)
        {
            if (customPlot->graph(i)->name() == variable)
            {
                customPlot->removeGraph(customPlot->graph(i));
                break;
            }
        }
    }

    redrawGraph();
}

void MainWindow::redrawGraph()
{
    customPlot->xAxis->setLabel("Время");
    customPlot->yAxis->setLabel("Значение");
    customPlot->legend->setVisible(true);
    customPlot->rescaleAxes(); // Автоматическое масштабирование осей
    customPlot->replot();      // Отрисовка графиков
}
