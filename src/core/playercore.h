#ifndef PLAYERCORE_H
#define PLAYERCORE_H

#include <QObject>
#include <QStringList>
#include <QUrl>
#include <QTimer>

// Qt 6.7 requires fully-defined types for Q_PROPERTY metatype registration
#include "core/mpvengine.h"

class HistoryModel;

class PlayerCore : public QObject
{
    Q_OBJECT

    Q_PROPERTY(MpvEngine* mpv READ mpv CONSTANT)
    Q_PROPERTY(bool fullscreen READ isFullscreen WRITE setFullscreen NOTIFY fullscreenChanged)
    Q_PROPERTY(bool playlistVisible READ isPlaylistVisible WRITE setPlaylistVisible NOTIFY playlistVisibleChanged)
    Q_PROPERTY(bool chaptersVisible READ isChaptersVisible WRITE setChaptersVisible NOTIFY chaptersVisibleChanged)
    Q_PROPERTY(bool historyVisible READ isHistoryVisible WRITE setHistoryVisible NOTIFY historyVisibleChanged)
    Q_PROPERTY(QString positionText READ positionText NOTIFY positionTextChanged)
    Q_PROPERTY(QString durationText READ durationText NOTIFY durationTextChanged)
    Q_PROPERTY(QString windowTitle READ windowTitle NOTIFY windowTitleChanged)
    Q_PROPERTY(bool resumePlayback READ resumePlayback WRITE setResumePlayback NOTIFY resumePlaybackChanged)

public:
    explicit PlayerCore(HistoryModel *history, QObject *parent = nullptr);

    MpvEngine *mpv() const { return m_mpv; }
    HistoryModel *historyModel() const { return m_history; }

    bool isFullscreen() const { return m_fullscreen; }
    void setFullscreen(bool fs);
    bool isPlaylistVisible() const { return m_playlistVisible; }
    void setPlaylistVisible(bool visible);
    bool isChaptersVisible() const { return m_chaptersVisible; }
    void setChaptersVisible(bool visible);
    bool isHistoryVisible() const { return m_historyVisible; }
    void setHistoryVisible(bool visible);
    QString positionText() const;
    QString durationText() const;
    QString windowTitle() const;
    bool resumePlayback() const { return m_resumePlayback; }
    void setResumePlayback(bool resume);

    Q_INVOKABLE void openFiles(const QList<QUrl> &urls);
    Q_INVOKABLE void openFile(const QString &path);
    Q_INVOKABLE void openUrl(const QString &url);
    Q_INVOKABLE void openSubtitleFile(const QUrl &url);
    Q_INVOKABLE void dropFiles(const QList<QUrl> &urls);
    Q_INVOKABLE void toggleFullscreen();
    Q_INVOKABLE void togglePlaylist();
    Q_INVOKABLE void toggleChapters();
    Q_INVOKABLE void toggleHistory();
    Q_INVOKABLE void openFromHistory(const QString &path);

    Q_INVOKABLE QVariantList getPlaylistItems() const;

    static QString formatTime(double seconds);

signals:
    void fullscreenChanged();
    void playlistVisibleChanged();
    void chaptersVisibleChanged();
    void historyVisibleChanged();
    void positionTextChanged();
    void durationTextChanged();
    void windowTitleChanged();
    void requestFullscreen(bool fs);
    void resumePlaybackChanged();
    void subtitleAutoLoaded(const QString &path);

private slots:
    void onFileLoaded();
    void onFileEnded();
    void saveHistoryEntry();

private:
    void autoLoadSubtitles(const QString &mediaPath);

    MpvEngine *m_mpv = nullptr;
    HistoryModel *m_history = nullptr;
    QTimer m_historySaveTimer;
    bool m_fullscreen = false;
    bool m_playlistVisible = false;
    bool m_chaptersVisible = false;
    bool m_historyVisible = false;
    bool m_resumePlayback = true;
};

#endif // PLAYERCORE_H
