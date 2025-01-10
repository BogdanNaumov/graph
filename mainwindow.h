#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>

QT_BEGIN_NAMESPACE
namespace Ui {
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
    // Слот для обработки открытия файла
    void openFile();

    // Слот для отображения/скрытия переменной на графике
    void toggleVariableOnGraph(const QString &variable, bool visible);

private:
    // Функция для обновления списка переменных (чекбоксов)
    void updateVariables(const QStringList &variables);

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
