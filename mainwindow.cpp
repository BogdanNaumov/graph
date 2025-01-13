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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    customPlot = new QCustomPlot(this);
    ui->plotArea->setLayout(new QVBoxLayout);
    ui->plotArea->layout()->addWidget(customPlot);

    connect(ui->actionOpenFile, &QAction::triggered, this, &MainWindow::openFile);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFile()
{

    QString filePath = QFileDialog::getOpenFileName(this, tr("Открыть файл"), "", tr("TAB Files (*.tab);;All Files (*)"));
    if (filePath.isEmpty())
        return;

    bool ok;
    int lineCount = QInputDialog::getInt(this, tr("Количество строк"), tr("Сколько строк парсить?"), 10, 1, 1000000, 1, &ok);
    if (!ok)
        return;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось открыть файл"));
        return;
    }

    QTextStream in(&file);
    QStringList variableNames;
    QVector<double> timeData;
    QMap<QString, QVector<double>> variablesData;

    if (!in.atEnd())
    {
        QString headerLine = in.readLine();
        variableNames = headerLine.split('\t', Qt::SkipEmptyParts);
        if (!variableNames.isEmpty() && variableNames.first() == "time")
        {
            variableNames.removeFirst();
        }
    }

    int parsedLines = 0;
    while (!in.atEnd() && parsedLines < lineCount)
    {
        QString line = in.readLine();
        QStringList values = line.split('\t', Qt::SkipEmptyParts);
        if (values.size() < variableNames.size() + 1)
            continue;

        bool ok = false;
        double timeValue = values[0].toDouble(&ok);
        if (!ok)
            continue;

        timeData.append(timeValue);
        for (int i = 0; i < variableNames.size(); ++i)
        {
            variablesData[variableNames[i]].append(values[i + 1].toDouble());
        }
        parsedLines++;
    }

    file.close();

    updateVariables(variableNames);

    this->timeData = timeData;
    this->variablesData = variablesData;

    redrawGraph();
}

void MainWindow::updateVariables(const QStringList &variables)
{

    QLayout *layout = ui->variablesLayout;
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr)
    {
        delete item->widget();
        delete item;
    }

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
    customPlot->rescaleAxes();
    customPlot->replot();
}
