#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>
#include <QQmlApplicationEngine>

class PlayerCore;
class PlaylistModel;
class ChapterModel;
class TrackModel;
class HistoryModel;
class ShortcutManager;
class SubtitleSearch;

class Application : public QObject
{
    Q_OBJECT
    Q_PROPERTY(PlayerCore* player READ player CONSTANT)
    Q_PROPERTY(PlaylistModel* playlistModel READ playlistModel CONSTANT)
    Q_PROPERTY(ChapterModel* chapterModel READ chapterModel CONSTANT)
    Q_PROPERTY(TrackModel* subtitleModel READ subtitleModel CONSTANT)
    Q_PROPERTY(TrackModel* audioModel READ audioModel CONSTANT)
    Q_PROPERTY(HistoryModel* historyModel READ historyModel CONSTANT)
    Q_PROPERTY(ShortcutManager* shortcuts READ shortcuts CONSTANT)
    Q_PROPERTY(SubtitleSearch* subtitleSearch READ subtitleSearch CONSTANT)
    Q_PROPERTY(QString version READ version CONSTANT)

public:
    explicit Application(QObject *parent = nullptr);

    PlayerCore *player() const { return m_player; }
    PlaylistModel *playlistModel() const { return m_playlistModel; }
    ChapterModel *chapterModel() const { return m_chapterModel; }
    TrackModel *subtitleModel() const { return m_subtitleModel; }
    TrackModel *audioModel() const { return m_audioModel; }
    HistoryModel *historyModel() const { return m_historyModel; }
    ShortcutManager *shortcuts() const { return m_shortcuts; }
    SubtitleSearch *subtitleSearch() const { return m_subtitleSearch; }
    QString version() const;

    void init(QQmlApplicationEngine &engine);
    void processCommandLine();

private:
    PlayerCore *m_player = nullptr;
    PlaylistModel *m_playlistModel = nullptr;
    ChapterModel *m_chapterModel = nullptr;
    TrackModel *m_subtitleModel = nullptr;
    TrackModel *m_audioModel = nullptr;
    HistoryModel *m_historyModel = nullptr;
    ShortcutManager *m_shortcuts = nullptr;
    SubtitleSearch *m_subtitleSearch = nullptr;
};

#endif // APPLICATION_H
