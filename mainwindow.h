#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QMap>
#include <QStringList>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openFile();
    void toggleVariableOnGraph(const QString &variable, bool visible);

private:
    void updateVariables(const QStringList &variables);
    void redrawGraph();

    Ui::MainWindow *ui;
    QCustomPlot *customPlot;
    QVector<double> timeData;
    QMap<QString, QVector<double>> variablesData;
};

#endif // MAINWINDOW_H
