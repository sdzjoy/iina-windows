#include "playercore.h"
#include "mpvengine.h"
#include "models/historymodel.h"
#include "utils/config.h"

#include <QFileInfo>
#include <QDir>
#include <QUrl>
#include <QDebug>
#include <cmath>

static const QStringList SUBTITLE_EXTENSIONS = {
    "srt", "ass", "ssa", "sub", "idx", "vtt", "lrc", "sup", "smi"
};

static const QStringList MEDIA_EXTENSIONS = {
    "mp4", "mkv", "avi", "webm", "mov", "flv", "wmv", "m4v", "mpg", "mpeg",
    "ts", "vob", "3gp", "mp3", "flac", "ogg", "wav", "aac", "m4a", "wma", "opus"
};

PlayerCore::PlayerCore(HistoryModel *history, QObject *parent)
    : QObject(parent)
    , m_mpv(new MpvEngine(this))
    , m_history(history)
{
    connect(m_mpv, &MpvEngine::positionChanged, this, &PlayerCore::positionTextChanged);
    connect(m_mpv, &MpvEngine::durationChanged, this, &PlayerCore::durationTextChanged);
    connect(m_mpv, &MpvEngine::mediaTitleChanged, this, &PlayerCore::windowTitleChanged);
    connect(m_mpv, &MpvEngine::filePathChanged, this, &PlayerCore::windowTitleChanged);
    connect(m_mpv, &MpvEngine::fileLoaded, this, &PlayerCore::onFileLoaded);
    connect(m_mpv, &MpvEngine::fileEnded, this, &PlayerCore::onFileEnded);

    // Periodically save history (every 10 seconds while playing)
    m_historySaveTimer.setInterval(10000);
    connect(&m_historySaveTimer, &QTimer::timeout, this, &PlayerCore::saveHistoryEntry);
    connect(m_mpv, &MpvEngine::playingChanged, this, [this]() {
        if (m_mpv->isPlaying()) m_historySaveTimer.start();
        else m_historySaveTimer.stop();
    });

    // Load resume setting
    m_resumePlayback = Config::instance().value("player/resumePlayback", true).toBool();
}

void PlayerCore::setFullscreen(bool fs)
{
    if (m_fullscreen != fs) {
        m_fullscreen = fs;
        emit fullscreenChanged();
        emit requestFullscreen(fs);
    }
}

void PlayerCore::setPlaylistVisible(bool visible)
{
    if (m_playlistVisible != visible) {
        m_playlistVisible = visible;
        emit playlistVisibleChanged();
    }
}

void PlayerCore::setChaptersVisible(bool visible)
{
    if (m_chaptersVisible != visible) {
        m_chaptersVisible = visible;
        emit chaptersVisibleChanged();
    }
}

void PlayerCore::setHistoryVisible(bool visible)
{
    if (m_historyVisible != visible) {
        m_historyVisible = visible;
        emit historyVisibleChanged();
    }
}

void PlayerCore::setResumePlayback(bool resume)
{
    if (m_resumePlayback != resume) {
        m_resumePlayback = resume;
        Config::instance().setValue("player/resumePlayback", resume);
        emit resumePlaybackChanged();
    }
}

QString PlayerCore::positionText() const
{
    return formatTime(m_mpv->position());
}

QString PlayerCore::durationText() const
{
    return formatTime(m_mpv->duration());
}

QString PlayerCore::windowTitle() const
{
    QString title = m_mpv->mediaTitle();
    if (title.isEmpty()) {
        QString path = m_mpv->filePath();
        if (!path.isEmpty()) {
            QFileInfo fi(path);
            title = fi.fileName();
        }
    }
    if (title.isEmpty()) return QStringLiteral("IINA");
    return title + QStringLiteral(" - IINA");
}

void PlayerCore::openFiles(const QList<QUrl> &urls)
{
    if (urls.isEmpty()) return;

    QString firstPath = urls.first().toLocalFile();
    if (firstPath.isEmpty())
        firstPath = urls.first().toString();
    m_mpv->loadFile(firstPath);

    for (int i = 1; i < urls.size(); i++) {
        QString path = urls[i].toLocalFile();
        if (path.isEmpty())
            path = urls[i].toString();
        m_mpv->playlistAppend(path);
    }
}

void PlayerCore::openFile(const QString &path)
{
    m_mpv->loadFile(path);
}

void PlayerCore::openUrl(const QString &url)
{
    m_mpv->loadUrl(url);
}

void PlayerCore::openSubtitleFile(const QUrl &url)
{
    QString path = url.toLocalFile();
    if (!path.isEmpty()) {
        m_mpv->addSubtitleFile(path);
    }
}

void PlayerCore::dropFiles(const QList<QUrl> &urls)
{
    if (urls.isEmpty()) return;

    QList<QUrl> mediaUrls;
    QList<QUrl> subUrls;

    for (const auto &url : urls) {
        QString path = url.toLocalFile();
        QFileInfo fi(path);
        if (SUBTITLE_EXTENSIONS.contains(fi.suffix().toLower())) {
            subUrls.append(url);
        } else {
            mediaUrls.append(url);
        }
    }

    // If only subtitle files dropped on playing video, add them
    if (mediaUrls.isEmpty() && !subUrls.isEmpty() && m_mpv->isPlaying()) {
        for (const auto &url : subUrls) {
            openSubtitleFile(url);
        }
        return;
    }

    if (!mediaUrls.isEmpty()) {
        openFiles(mediaUrls);
        // Also load any subtitle files that came with the drop
        for (const auto &url : subUrls) {
            openSubtitleFile(url);
        }
    }
}

void PlayerCore::toggleFullscreen()
{
    setFullscreen(!m_fullscreen);
}

void PlayerCore::togglePlaylist()
{
    setPlaylistVisible(!m_playlistVisible);
}

void PlayerCore::toggleChapters()
{
    setChaptersVisible(!m_chaptersVisible);
}

void PlayerCore::toggleHistory()
{
    setHistoryVisible(!m_historyVisible);
}

void PlayerCore::openFromHistory(const QString &path)
{
    m_mpv->loadFile(path);
    setHistoryVisible(false);
}

QVariantList PlayerCore::getPlaylistItems() const
{
    return m_mpv->getProperty("playlist").toList();
}

void PlayerCore::onFileLoaded()
{
    QString path = m_mpv->filePath();

    // Auto-load subtitles from same directory
    autoLoadSubtitles(path);

    // Resume from last position
    if (m_resumePlayback && m_history) {
        double resumePos = m_history->getResumePosition(path);
        if (resumePos > 2.0) {
            m_mpv->seekAbsolute(resumePos);
        }
    }
}

void PlayerCore::onFileEnded()
{
    saveHistoryEntry();
}

void PlayerCore::saveHistoryEntry()
{
    if (!m_history || !m_mpv->isPlaying()) return;

    QString path = m_mpv->filePath();
    if (path.isEmpty()) return;

    m_history->addEntry(path, m_mpv->mediaTitle(),
                        m_mpv->duration(), m_mpv->position());
}

void PlayerCore::autoLoadSubtitles(const QString &mediaPath)
{
    if (mediaPath.isEmpty()) return;

    QFileInfo mediaInfo(mediaPath);
    if (!mediaInfo.exists()) return; // Network stream

    QDir dir = mediaInfo.dir();
    QString baseName = mediaInfo.completeBaseName();

    // Look for subtitle files with matching base name
    QStringList nameFilters;
    for (const auto &ext : SUBTITLE_EXTENSIONS) {
        nameFilters << (baseName + ".*." + ext);
        nameFilters << (baseName + "." + ext);
    }

    QStringList matchingFiles = dir.entryList(nameFilters, QDir::Files, QDir::Name);
    for (const auto &subFile : matchingFiles) {
        QString subPath = dir.absoluteFilePath(subFile);
        // mpv may have already loaded it via sub-auto, but adding it won't hurt
        qDebug() << "Auto-loading subtitle:" << subPath;
        emit subtitleAutoLoaded(subPath);
    }
}

QString PlayerCore::formatTime(double seconds)
{
    if (std::isnan(seconds) || seconds < 0) seconds = 0;
    int totalSecs = static_cast<int>(seconds);
    int hours = totalSecs / 3600;
    int mins = (totalSecs % 3600) / 60;
    int secs = totalSecs % 60;

    if (hours > 0)
        return QString::asprintf("%d:%02d:%02d", hours, mins, secs);
    else
        return QString::asprintf("%02d:%02d", mins, secs);
}
