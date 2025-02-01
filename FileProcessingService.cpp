#include "FileProcessingService.h"
#include "worddefinitionfetcher.h"
#include <QFile>
#include <QTextStream>
#include <QEventLoop>
#include <QObject>
#include <stdexcept>

/**
 * @brief Helper function to fetch a word's definition synchronously.
 * Internally uses a QEventLoop to wait for the asynchronous API response.
 * @param word The word for which to fetch the definition.
 * @return The fetched definition, or an error message if the fetch fails.
 */
static QString fetchDefinitionSync(const QString &word) {
    QEventLoop loop;
    QString definition;
    WordDefinitionFetcher fetcher;
    QObject::connect(&fetcher, &WordDefinitionFetcher::definitionFetched,
                     [&loop, &definition, word](const QString &fetchedWord, const QString &def) {
                         if (fetchedWord == word) {
                             definition = def;
                             loop.quit();
                         }
                     });
    QObject::connect(&fetcher, &WordDefinitionFetcher::fetchFailed,
                     [&loop, &definition, word](const QString &fetchedWord, const QString &error) {
                         if (fetchedWord == word) {
                             definition = QString("Error: %1").arg(error);
                             loop.quit();
                         }
                     });
    fetcher.fetchDefinition(word);
    loop.exec();
    return definition;
}

FileProcessingService::FileProcessingService(IFileParser *parser)
    : parser(parser)
{
}

// Processes the file and obtains definitions for each word.
QList<QPair<QString, QString>> FileProcessingService::processFileWithDefinitions()
{
    QList<QPair<QString, QString>> results;
    QStringList words = parser->parseWords();

    // For each word, fetch its definition synchronously.
    for (const QString &word : words) {
        QString def = fetchDefinitionSync(word);
        results.append(qMakePair(word, def));
    }
    return results;
}

QString FileProcessingService::saveProcessedWordsWithDefinitions(const QString &outputPath,
                                                                 const QList<QPair<QString, QString>> &wordDefinitions)
{
    QFile outputFile(outputPath);
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        throw std::runtime_error("FileProcessingService: Failed to save file.");
    }

    QTextStream out(&outputFile);
    // Write each word-definition pair on a separate block, separated by a double newline.
    for (const auto &pair : wordDefinitions) {
        out << pair.first << " : " << pair.second << "\n\n";
    }
    outputFile.close();
    return outputPath;
}

