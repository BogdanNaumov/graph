#include "mainwindow.h"
#include "fileparser.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QCheckBox>
#include <QVector>
#include <QMap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    fileParser = new FileParser(this);

    customPlot = new QCustomPlot(this);
    ui->plotArea->setLayout(new QVBoxLayout);
    ui->plotArea->layout()->addWidget(customPlot);

    connect(ui->actionOpenFile, &QAction::triggered, this, &MainWindow::openFile);
    connect(fileParser, &FileParser::fileParsed, this, &MainWindow::onFileParsed);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete fileParser;
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

    fileParser->parseFile(filePath, lineCount);
}

void MainWindow::onFileParsed()
{
    updateVariables(fileParser->getVariableNames());
    timeData = fileParser->getTimeData();
    variablesData = fileParser->getVariablesData();
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

        static int colorIndex = 0;
        const QList<QColor> colors = {
            QColor(Qt::red), QColor(Qt::green), QColor(Qt::blue), QColor(Qt::cyan),
            QColor(Qt::magenta), QColor(Qt::yellow), QColor(Qt::gray)
        };

        QColor color = colors[colorIndex % colors.size()];
        colorIndex++;

        QPen pen(color);
        pen.setWidth(2);
        graph->setPen(pen);
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
