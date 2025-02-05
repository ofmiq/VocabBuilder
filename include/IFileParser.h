#ifndef IFILEPARSER_H
#define IFILEPARSER_H

#include <QString>
#include <QStringList>

/**
 * @brief The IFileParser interface
 * Interface for parsing files into a list of words.
 */
class IFileParser
{
public:
    virtual ~IFileParser() = default;

    /**
     * @brief Checks if the file is valid (exists and is non-empty).
     * @return true if valid, false otherwise.
     */
    virtual bool isValid() const = 0;

    /**
     * @brief Parses the file and returns a list of words.
     * @return QStringList containing all parsed words.
     */
    virtual QStringList parseWords() = 0;
};

#endif // IFILEPARSER_H
