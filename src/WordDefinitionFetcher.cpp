#include "WordDefinitionFetcher.h"
#include <QUrl>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

WordDefinitionFetcher::WordDefinitionFetcher(QObject *parent)
    : QObject(parent)
{
    // Connect the finished signal of the network manager to the reply handler slot.
    connect(&networkManager, &QNetworkAccessManager::finished,
            this, &WordDefinitionFetcher::onReplyFinished);
}

void WordDefinitionFetcher::fetchDefinition(const QString &word)
{
    // Construct the API URL
    QString baseUrl = "https://api.dictionaryapi.dev/api/v2/entries/en/" + word;
    QUrl url(baseUrl);
    QNetworkRequest request(url);
    networkManager.get(request);
}

void WordDefinitionFetcher::onReplyFinished(QNetworkReply *reply)
{
    // Extract the word from the URL (assumes the word is the last segment)
    QUrl url = reply->url();
    QString word = url.path().section('/', -1);

    if (reply->error() != QNetworkReply::NoError) {
        QString errorString = reply->errorString();
        QNetworkReply::NetworkError errorType = reply->error();

        if (reply->error() == QNetworkReply::NetworkError::ContentNotFoundError) { // Check for 404
            emit fetchFailed(word, "No definition found");
        } else if (errorType == QNetworkReply::NetworkError::TimeoutError) {
            emit fetchFailed(word, "Network timeout error");
        } else if (errorType == QNetworkReply::NetworkError::ConnectionRefusedError ||
                   errorType == QNetworkReply::NetworkError::HostNotFoundError) {
            emit fetchFailed(word, "Network connection error");
        } else {
            // Check for specific HTTP status codes.  404 means "Not Found"
            if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 404) {
                emit fetchFailed(word, "No definition found");
            } else {
                emit fetchFailed(word, QString("Network error: %1 (HTTP Status: %2)").arg(errorString).arg(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()));
            }
        }

        reply->deleteLater();
        return;
    }

    QByteArray responseData = reply->readAll();
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData, &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        qDebug() << "JSON Parse error for word:" << word << ", error:" << jsonError.errorString();
        emit fetchFailed(word, QString("JSON Parse error: %1").arg(jsonError.errorString())); // JSON parsing error
        reply->deleteLater();
        return;
    }

    if (!jsonDoc.isArray()) {
        qDebug() << "Unexpected JSON format for word:" << word;
        emit fetchFailed(word, "Unexpected JSON format from API"); // Unexpected JSON format
        reply->deleteLater();
        return;
    }

    QJsonArray jsonArray = jsonDoc.array();
    QStringList formattedDefinitions; // Will hold formatted definition lines for each part of speech

    // Iterate over each entry in the array.
    for (int i = 0; i < jsonArray.size(); ++i) {
        QJsonObject entry = jsonArray[i].toObject();
        QJsonArray meanings = entry.value("meanings").toArray();

        // For each meaning, corresponding to a specific part of speech.
        for (int j = 0; j < meanings.size(); ++j) {
            QJsonObject meaningObj = meanings[j].toObject();
            QString partOfSpeech = meaningObj.value("partOfSpeech").toString();
            QJsonArray definitions = meaningObj.value("definitions").toArray();

            // If there is at least one definition, take the first one.
            if (!definitions.isEmpty()) {
                QJsonObject firstDefObj = definitions.first().toObject();
                QString defText = firstDefObj.value("definition").toString();
                if (!defText.isEmpty()) {
                    // Format: "- partOfSpeech: definition"
                    formattedDefinitions.append(QString("- %1: %2").arg(partOfSpeech, defText));
                }
            }
        }
    }

    // Build the combined definitions string with newline separation.
    QString combinedDefinitions;
    if (formattedDefinitions.isEmpty()) {
        combinedDefinitions = "No definition found";
    } else {
        combinedDefinitions = formattedDefinitions.join("\n");
    }

    emit definitionFetched(word, combinedDefinitions);
    reply->deleteLater();
}
