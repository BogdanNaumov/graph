#ifndef FILEPARSER_H
#define FILEPARSER_H

#include <QString>
#include <QStringList>
#include <QVector>
#include <QMap>
#include <QObject>

class FileParser : public QObject
{
    Q_OBJECT

public:
    explicit FileParser(QObject *parent = nullptr);

    bool parseFile(const QString &filePath, int lineCount);

    QStringList getVariableNames() const;
    QVector<double> getTimeData() const;
    QMap<QString, QVector<double>> getVariablesData() const;

signals:
    void fileParsed(); // Сигнал для уведомления о завершении загрузки данных

private:
    QStringList variableNames;
    QVector<double> timeData;
    QMap<QString, QVector<double>> variablesData;
};

#endif // FILEPARSER_H
