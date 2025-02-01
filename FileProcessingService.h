#ifndef FILEPROCESSINGSERVICE_H
#define FILEPROCESSINGSERVICE_H

#include "IFileParser.h"
#include <QString>
#include <QStringList>
#include <QPair>
#include <QList>

/**
 * @brief The FileProcessingService class
 * Provides services for processing a file and obtaining definitions for each word.
 */
class FileProcessingService
{
public:
    /**
     * @brief Constructor.
     * @param parser Pointer to an IFileParser instance.
     */
    explicit FileProcessingService(IFileParser *parser);

    /**
     * @brief Processes the file and returns a list of <word, definition> pairs.
     * For each word parsed from the file, the definition is fetched from the API.
     * @return QList of QPair, where first is the word and second is its definition.
     */
    QList<QPair<QString, QString>> processFileWithDefinitions();

    /**
     * @brief Saves the processed word-definition pairs to a file.
     * Each line of the file will have the format: word : definition
     * @param outputPath The destination file path.
     * @param wordDefinitions The list of word-definition pairs.
     * @return The output path if saving is successful.
     * @throws std::runtime_error if the file cannot be saved.
     */
    QString saveProcessedWordsWithDefinitions(const QString &outputPath,
                                              const QList<QPair<QString, QString>> &wordDefinitions);

private:
    IFileParser *parser;  ///< Parser used to extract words from a file.
};

#endif // FILEPROCESSINGSERVICE_H
