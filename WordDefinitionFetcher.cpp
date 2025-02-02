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
        emit fetchFailed(word, reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray responseData = reply->readAll();
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData, &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        emit fetchFailed(word, jsonError.errorString());
        reply->deleteLater();
        return;
    }

    if (!jsonDoc.isArray()) {
        emit fetchFailed(word, "Unexpected JSON format");
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
