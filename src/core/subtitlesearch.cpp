#include "subtitlesearch.h"

#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QFile>
#include <QDir>
#include <QStandardPaths>

// Uses the OpenSubtitles REST API (v1)
// See: https://opensubtitles.stoplight.io/docs/opensubtitles-api
// Note: full functionality requires an API key (free tier available)

static const QString API_BASE = "https://api.opensubtitles.com/api/v1";

SubtitleSearch::SubtitleSearch(QObject *parent)
    : QObject(parent)
{
}

void SubtitleSearch::searchByQuery(const QString &query, const QString &lang)
{
    if (query.isEmpty()) return;

    m_searching = true;
    m_errorMessage.clear();
    m_results.clear();
    emit searchingChanged();
    emit resultsChanged();
    emit errorChanged();

    QUrl url(API_BASE + "/subtitles");
    QUrlQuery params;
    params.addQueryItem("query", query);
    params.addQueryItem("languages", lang);
    params.addQueryItem("order_by", "download_count");
    params.addQueryItem("order_direction", "desc");
    url.setQuery(params);

    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Api-Key", "");  // User must provide their own API key
    req.setRawHeader("User-Agent", "IINA-Windows v0.1.0");

    auto *reply = m_nam.get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        m_searching = false;
        emit searchingChanged();

        if (reply->error() != QNetworkReply::NoError) {
            // Provide helpful message if API key is missing
            if (reply->error() == QNetworkReply::AuthenticationRequiredError) {
                m_errorMessage = tr("Subtitle search requires an OpenSubtitles API key. "
                                    "Get one free at opensubtitles.com");
            } else {
                m_errorMessage = reply->errorString();
            }
            emit errorChanged();
            return;
        }

        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonArray data = doc.object()["data"].toArray();

        m_results.clear();
        for (const auto &item : data) {
            QJsonObject obj = item.toObject();
            QJsonObject attrs = obj["attributes"].toObject();

            QVariantMap result;
            result["id"] = obj["id"].toString();
            result["title"] = attrs["release"].toString();
            result["language"] = attrs["language"].toString();
            result["downloads"] = attrs["download_count"].toInt();
            result["fps"] = attrs["fps"].toDouble();
            result["hearing_impaired"] = attrs["hearing_impaired"].toBool();

            // Extract download file info
            QJsonArray files = attrs["files"].toArray();
            if (!files.isEmpty()) {
                QJsonObject file = files[0].toObject();
                result["file_id"] = file["file_id"].toInt();
                result["file_name"] = file["file_name"].toString();
            }

            m_results.append(result);
        }

        emit resultsChanged();
    });
}

void SubtitleSearch::downloadSubtitle(const QString &url, const QString &savePath)
{
    QNetworkRequest req{QUrl(url)};
    req.setRawHeader("User-Agent", "IINA-Windows v0.1.0");

    auto *reply = m_nam.get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply, savePath]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            emit downloadFailed(reply->errorString());
            return;
        }

        QFile file(savePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(reply->readAll());
            file.close();
            emit downloadCompleted(savePath);
        } else {
            emit downloadFailed(tr("Could not save subtitle file"));
        }
    });
}

void SubtitleSearch::clear()
{
    m_results.clear();
    m_errorMessage.clear();
    emit resultsChanged();
    emit errorChanged();
}
