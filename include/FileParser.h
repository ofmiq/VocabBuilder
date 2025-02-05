#ifndef FILEPARSER_H
#define FILEPARSER_H

#include "IFileParser.h"
#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>

/**
 * @brief The FileParser class
 * Implementation of IFileParser for plain text files.
 */
class FileParser : public IFileParser
{
public:
    /**
     * @brief Constructor.
     * @param filePath The path to the file to parse.
     */
    explicit FileParser(const QString &filePath);

    /**
     * @brief Checks if the file exists and is non-empty.
     * @return true if valid, false otherwise.
     */
    bool isValid() const override;

    /**
     * @brief Parses the file and returns a list of words.
     * Uses a regular expression to split the text into words.
     * @return QStringList of words.
     */
    QStringList parseWords() override;

private:
    QString filePath;  ///< The path to the file.
    bool valid;        ///< Flag indicating file validity.

    /**
     * @brief Validates the file.
     * @return true if the file exists and is non-empty.
     */
    bool validateFile();
};

#endif // FILEPARSER_H
