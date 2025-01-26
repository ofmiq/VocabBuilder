#ifndef IFILEPARSER_H
#define IFILEPARSER_H

#include <QString>
#include <QStringList>

// Интерфейс для парсинга файлов
class IFileParser
{
public:
    virtual ~IFileParser() = default;
    virtual bool isValid() const = 0;
    virtual QStringList parseWords() = 0;
};

#endif // IFILEPARSER_H
