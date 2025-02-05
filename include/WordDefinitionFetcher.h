#ifndef WORDDEFINITIONFETCHER_H
#define WORDDEFINITIONFETCHER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>

/**
 * @brief The WordDefinitionFetcher class
 * Fetches the definition of a word using the Free Dictionary API.
 */
class WordDefinitionFetcher : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Constructor.
     * @param parent The parent QObject.
     */
    explicit WordDefinitionFetcher(QObject *parent = nullptr);

    /**
     * @brief Initiates an API request to fetch the definition of a word.
     * @param word The word for which to fetch the definition.
     */
    void fetchDefinition(const QString &word);

signals:
    /**
     * @brief Emitted when a definition is successfully fetched.
     * @param word The word.
     * @param definition The fetched definition (formatted with part-of-speech info).
     */
    void definitionFetched(const QString &word, const QString &definition);

    /**
     * @brief Emitted when fetching a definition fails.
     * @param word The word.
     * @param error The error message.
     */
    void fetchFailed(const QString &word, const QString &error);

private slots:
    /**
     * @brief Handles the network reply.
     * Parses the JSON response and extracts definitions.
     * @param reply The network reply.
     */
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager networkManager; ///< Manager for handling network requests.
};

#endif // WORDDEFINITIONFETCHER_H
