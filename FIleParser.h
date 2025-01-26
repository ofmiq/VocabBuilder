#ifndef FILEPARSER_H
#define FILEPARSER_H

#include "IFileParser.h"
#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>

// Реализация IFileParser для текстовых файлов
class FileParser : public IFileParser
{
public:
    explicit FileParser(const QString &filePath);
    bool isValid() const override;
    QStringList parseWords() override;

private:
    QString filePath;
    bool valid;

    bool validateFile();
};

#endif // FILEPARSER_H
