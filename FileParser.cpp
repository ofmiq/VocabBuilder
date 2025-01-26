#include "FileParser.h"

FileParser::FileParser(const QString &filePath) : filePath(filePath), valid(false)
{
    valid = validateFile();
}

bool FileParser::isValid() const
{
    return valid;
}

QStringList FileParser::parseWords()
{
    QStringList wordList;
    if (!valid) {
        throw std::runtime_error("FileParser: File is not valid.");
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error("FileParser: Failed to open file.");
    }

    QTextStream in(&file);
    QRegularExpression wordRegex("\\w+");

    while (!in.atEnd()) {
        QString line = in.readLine();
        QRegularExpressionMatchIterator matchIterator = wordRegex.globalMatch(line);

        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            wordList.append(match.captured());
        }
    }

    file.close();
    return wordList;
}

bool FileParser::validateFile()
{
    QFile file(filePath);
    return file.exists() && file.size() > 0;
}
