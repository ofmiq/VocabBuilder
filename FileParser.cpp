#include "FileParser.h"

FileParser::FileParser(const QString &filePath)
    : filePath(filePath), valid(false)
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
        qDebug() << "FileParser: File is not valid.";
        return wordList;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "FileParser: Failed to open file.";
        return wordList;
    }

    QTextStream in(&file);
    // Regular expression to match words (alphanumeric characters)
    QRegularExpression wordRegex("\\w+");

    // Read file line by line and extract words
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
