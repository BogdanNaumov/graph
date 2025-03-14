#include "fileparser.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

FileParser::FileParser(QObject *parent) : QObject(parent) {}

bool FileParser::parseFile(const QString &filePath, int lineCount)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(nullptr, "Ошибка", "Не удалось открыть файл");
        return false;
    }

    QTextStream in(&file);
    variableNames.clear();
    timeData.clear();
    variablesData.clear();

    if (!in.atEnd())
    {
        QString headerLine = in.readLine();
        variableNames = headerLine.split('\t', Qt::SkipEmptyParts);

        if (!variableNames.isEmpty() && variableNames.last() == "dt")
        {
            variableNames.removeLast();
        }
    }

    int parsedLines = 0;
    double currentTime = 0.0;
    while (!in.atEnd() && parsedLines < lineCount)
    {
        QString line = in.readLine();
        QStringList values = line.split('\t', Qt::SkipEmptyParts);
        if (values.size() < variableNames.size() + 1)
            continue;

        timeData.append(currentTime);
        double dt = values.last().toDouble();
        currentTime += dt;

        for (int i = 0; i < variableNames.size(); ++i)
        {
            variablesData[variableNames[i]].append(values[i].toDouble());
        }
        parsedLines++;
    }

    file.close();

    emit fileParsed();
    return true;
}

QStringList FileParser::getVariableNames() const { return variableNames; }
QVector<double> FileParser::getTimeData() const { return timeData; }
QMap<QString, QVector<double>> FileParser::getVariablesData() const { return variablesData; }
