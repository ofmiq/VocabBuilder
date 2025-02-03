#ifndef FILEPROCESSINGSERVICE_H
#define FILEPROCESSINGSERVICE_H

#include "IFileParser.h"
#include <QString>
#include <QStringList>
#include <QPair>
#include <QList>
#include <memory>

/**
 * @brief The FileProcessingService class
 * Provides services for processing a file and obtaining definitions for each word.
 */
class FileProcessingService
{
public:
    /**
     * @brief Constructor.
     * @param parser Unique pointer to an IFileParser instance.
     */
    explicit FileProcessingService(std::unique_ptr<IFileParser> parser);

    /**
     * @brief Returns the list of words parsed from the file.
     * @return QStringList of words.
     */
    QStringList getWords() const;

    /**
     * @brief Processes the file and returns a list of <word, definition> pairs.
     * For each word parsed from the file, the definition is fetched from the API concurrently.
     * @return QList of QPair, where first is the word and second is its definition.
     */
    QList<QPair<QString, QString>> processFileWithDefinitions();

    /**
     * @brief Saves the processed word-definition pairs to a plain text file.
     * Each block in the file will have the format:
     * word :
     * definition
     *
     * @param outputPath The destination file path.
     * @param wordDefinitions The list of word-definition pairs.
     * @return The output path if saving is successful.
     * @throws std::runtime_error if the file cannot be saved.
     */
    QString saveProcessedWordsWithDefinitions(const QString &outputPath,
                                              const QList<QPair<QString, QString>> &wordDefinitions);

    /**
     * @brief Saves the processed word-definition pairs to a CSV file.
     * The CSV will have a header and each subsequent line will contain the word and its definition.
     *
     * @param outputPath The destination CSV file path.
     * @param wordDefinitions The list of word-definition pairs.
     * @return The output path if saving is successful.
     * @throws std::runtime_error if the file cannot be saved.
     */
    QString saveProcessedWordsToCsv(const QString &outputPath,
                                    const QList<QPair<QString, QString>> &wordDefinitions);

    /**
     * @brief Fetches the definition of a word synchronously.
     * Implements a retry mechanism with exponential backoff if a transfer error occurs.
     * @param word The word for which to fetch the definition.
     * @return The fetched definition.
     */
    QString fetchDefinitionSync(const QString &word);

private:
    std::unique_ptr<IFileParser> parser;  ///< Parser used to extract words from a file.
};

#endif // FILEPROCESSINGSERVICE_H
