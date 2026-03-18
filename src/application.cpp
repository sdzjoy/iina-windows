#include "application.h"
#include "core/playercore.h"
#include "core/mpvengine.h"
#include "core/mpvrenderer.h"
#include "core/shortcutmanager.h"
#include "core/subtitlesearch.h"
#include "models/playlistmodel.h"
#include "models/chaptermodel.h"
#include "models/trackmodel.h"
#include "models/historymodel.h"
#include "utils/config.h"

#include <QCoreApplication>
#include <QQmlContext>
#include <QCommandLineParser>
#include <QFileInfo>
#include <QUrl>

Application::Application(QObject *parent)
    : QObject(parent)
    , m_historyModel(new HistoryModel(this))
    , m_shortcuts(new ShortcutManager(this))
    , m_player(new PlayerCore(m_historyModel, this))
    , m_playlistModel(new PlaylistModel(m_player->mpv(), this))
    , m_chapterModel(new ChapterModel(m_player->mpv(), this))
    , m_subtitleModel(new TrackModel(m_player->mpv(), "sub", this))
    , m_audioModel(new TrackModel(m_player->mpv(), "audio", this))
    , m_subtitleSearch(new SubtitleSearch(this))
{
    // Restore saved settings
    Config &cfg = Config::instance();
    m_player->mpv()->setVolume(cfg.volume());
    m_player->mpv()->setMuted(cfg.isMuted());

    // Restore screenshot settings
    QString ssFormat = cfg.value("screenshot/format", "png").toString();
    m_player->mpv()->setScreenshotFormat(ssFormat);
    QString ssDir = cfg.value("screenshot/directory", "").toString();
    if (!ssDir.isEmpty()) m_player->mpv()->setScreenshotDirectory(ssDir);

    // Restore subtitle settings
    QString subFont = cfg.value("subtitle/font", "").toString();
    if (!subFont.isEmpty()) m_player->mpv()->setSubtitleFont(subFont);
    double subScale = cfg.value("subtitle/scale", 1.0).toDouble();
    if (subScale != 1.0) m_player->mpv()->setSubtitleScale(subScale);
    QString subColor = cfg.value("subtitle/color", "").toString();
    if (!subColor.isEmpty()) m_player->mpv()->setSubtitleColor(subColor);

    // Save volume on change
    connect(m_player->mpv(), &MpvEngine::volumeChanged, this, [this]() {
        Config::instance().setVolume(m_player->mpv()->volume());
    });
    connect(m_player->mpv(), &MpvEngine::mutedChanged, this, [this]() {
        Config::instance().setMuted(m_player->mpv()->isMuted());
    });
}

QString Application::version() const
{
    return QCoreApplication::applicationVersion();
}

void Application::init(QQmlApplicationEngine &engine)
{
    // Register C++ types with QML
    qmlRegisterType<MpvObject>("IINAWindows", 1, 0, "MpvObject");

    // Expose application singleton to QML
    engine.rootContext()->setContextProperty("app", this);
    engine.rootContext()->setContextProperty("player", m_player);
    engine.rootContext()->setContextProperty("mpv", m_player->mpv());
    engine.rootContext()->setContextProperty("playlistModel", m_playlistModel);
    engine.rootContext()->setContextProperty("chapterModel", m_chapterModel);
    engine.rootContext()->setContextProperty("subtitleModel", m_subtitleModel);
    engine.rootContext()->setContextProperty("audioModel", m_audioModel);
    engine.rootContext()->setContextProperty("historyModel", m_historyModel);
    engine.rootContext()->setContextProperty("shortcuts", m_shortcuts);
    engine.rootContext()->setContextProperty("subtitleSearch", m_subtitleSearch);

    engine.loadFromModule("IINAWindows", "Main");
}

void Application::processCommandLine()
{
    QCommandLineParser parser;
    parser.setApplicationDescription("IINA for Windows - Modern video player");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("files", "Media files to play", "[files...]");

    parser.process(*QCoreApplication::instance());

    QStringList args = parser.positionalArguments();
    if (!args.isEmpty()) {
        QList<QUrl> urls;
        for (const auto &arg : args) {
            QFileInfo fi(arg);
            if (fi.exists()) {
                urls.append(QUrl::fromLocalFile(fi.absoluteFilePath()));
            } else {
                urls.append(QUrl(arg));
            }
        }
        m_player->openFiles(urls);
    }
}
