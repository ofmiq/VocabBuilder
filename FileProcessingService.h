#ifndef FILEPROCESSINGSERVICE_H
#define FILEPROCESSINGSERVICE_H

#include "IFileParser.h"
#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>

// Сервис для обработки файлов и сохранения результатов
class FileProcessingService
{
public:
    explicit FileProcessingService(IFileParser *parser);
    QStringList processFile();
    QString saveProcessedWords(const QString &outputPath, const QStringList &words);

private:
    IFileParser *parser;
};

#endif // FILEPROCESSINGSERVICE_H
