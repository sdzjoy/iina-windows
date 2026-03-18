#ifndef SUBTITLESEARCH_H
#define SUBTITLESEARCH_H

#include <QObject>
#include <QVariantList>
#include <QNetworkAccessManager>

class SubtitleSearch : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool searching READ isSearching NOTIFY searchingChanged)
    Q_PROPERTY(QVariantList results READ results NOTIFY resultsChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorChanged)

public:
    explicit SubtitleSearch(QObject *parent = nullptr);

    bool isSearching() const { return m_searching; }
    QVariantList results() const { return m_results; }
    QString errorMessage() const { return m_errorMessage; }

    // Search by file hash (best match) or by query string
    Q_INVOKABLE void searchByQuery(const QString &query, const QString &lang = "en");
    Q_INVOKABLE void downloadSubtitle(const QString &url, const QString &savePath);
    Q_INVOKABLE void clear();

signals:
    void searchingChanged();
    void resultsChanged();
    void errorChanged();
    void downloadCompleted(const QString &filePath);
    void downloadFailed(const QString &error);

private:
    QNetworkAccessManager m_nam;
    bool m_searching = false;
    QVariantList m_results;
    QString m_errorMessage;
};

#endif // SUBTITLESEARCH_H
