#include "FileProcessingService.h"

FileProcessingService::FileProcessingService(IFileParser *parser) : parser(parser) {}

QStringList FileProcessingService::processFile()
{
    if (!parser->isValid()) {
        throw std::runtime_error("FileProcessingService: Invalid file.");
    }
    return parser->parseWords();
}

QString FileProcessingService::saveProcessedWords(const QString &outputPath, const QStringList &words)
{
    QFile outputFile(outputPath);
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        throw std::runtime_error("FileProcessingService: Failed to save file.");
    }

    QTextStream out(&outputFile);
    for (const QString &word : words) {
        out << word << "\n";
    }
    outputFile.close();
    return outputPath;
}
