#include "FileProcessingService.h"
#include "WordDefinitionFetcher.h"
#include <QFile>
#include <QTextStream>
#include <QEventLoop>
#include <QObject>
#include <stdexcept>
#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>

FileProcessingService::FileProcessingService(std::unique_ptr<IFileParser> parser)
    : parser(std::move(parser))
{
}

QStringList FileProcessingService::getWords() const
{
    return parser->parseWords();
}

// Synchronously fetch the definition for a word using WordDefinitionFetcher,
// with a retry mechanism using exponential backoff for "Error transferring" errors.
QString FileProcessingService::fetchDefinitionSync(const QString &word)
{
    const int maxRetries = 5;
    int attempt = 0;
    QString definition;

    while (attempt < maxRetries) {
        QEventLoop loop;
        WordDefinitionFetcher fetcher;
        QObject::connect(&fetcher, &WordDefinitionFetcher::definitionFetched,
                         [&loop, &definition, word](const QString &fetchedWord, const QString &defText) {
                             if (fetchedWord.compare(word, Qt::CaseInsensitive) == 0) {
                                 definition = defText;
                                 loop.quit();
                             }
                         });
        QObject::connect(&fetcher, &WordDefinitionFetcher::fetchFailed,
                         [&loop, &definition, word](const QString &fetchedWord, const QString &error) {
                             if (fetchedWord.compare(word, Qt::CaseInsensitive) == 0) {
                                 definition = QString("Error: %1").arg(error);
                                 loop.quit();
                             }
                         });
        fetcher.fetchDefinition(word);
        loop.exec();

        // If no error occurred, break out of the loop.
        if (!definition.startsWith("Error:"))
            break;

        // If error indicates a transfer problem, apply exponential backoff and retry.
        if (definition.contains("Error transferring")) {
            attempt++;
            int delay = 1 << (attempt - 1);
            QThread::sleep(delay);
            continue;
        }
        break;
    }
    return definition;
}

// Processes the file and obtains definitions for each word concurrently.
QList<QPair<QString, QString>> FileProcessingService::processFileWithDefinitions()
{
    QList<QPair<QString, QString>> results;
    QStringList words = parser->parseWords();

    // Use QtConcurrent to fetch definitions concurrently.
    QFuture<QString> future = QtConcurrent::mapped(words, [this](const QString &word) {
        return fetchDefinitionSync(word);
    });
    QFutureWatcher<QString> watcher;
    watcher.setFuture(future);
    watcher.waitForFinished();
    QStringList definitions = watcher.future().results();

    // Combine words with their definitions.
    for (int i = 0; i < words.size(); ++i) {
        results.append(qMakePair(words[i], definitions[i]));
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
    // Write each word-definition block, separated by a double newline.
    for (const auto &pair : wordDefinitions) {
        out << pair.first << " :\n" << pair.second << "\n\n";
    }
    outputFile.close();
    return outputPath;
}

QString FileProcessingService::saveProcessedWordsToCsv(const QString &outputPath,
                                                       const QList<QPair<QString, QString>> &wordDefinitions)
{
    QFile outputFile(outputPath);
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        throw std::runtime_error("FileProcessingService: Failed to save CSV file.");
    }

    QTextStream out(&outputFile);
    // Write CSV header.
    out << "\"Word\",\"Definition\"\n";
    for (const auto &pair : wordDefinitions) {
        QString word = pair.first;
        QString definition = pair.second;
        // Escape quotes by doubling them.
        word.replace("\"", "\"\"");
        definition.replace("\"", "\"\"");
        out << "\"" << word << "\",\"" << definition << "\"\n";
    }
    outputFile.close();
    return outputPath;
}
