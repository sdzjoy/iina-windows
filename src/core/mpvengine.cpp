#include "mpvengine.h"

#include <QDebug>
#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDir>

#include <mpv/client.h>

// Helper: convert mpv_node to QVariant
static QVariant mpvNodeToVariant(const mpv_node &node)
{
    switch (node.format) {
    case MPV_FORMAT_STRING:
        return QVariant(QString::fromUtf8(node.u.string));
    case MPV_FORMAT_FLAG:
        return QVariant(static_cast<bool>(node.u.flag));
    case MPV_FORMAT_INT64:
        return QVariant(static_cast<qlonglong>(node.u.int64));
    case MPV_FORMAT_DOUBLE:
        return QVariant(node.u.double_);
    case MPV_FORMAT_NODE_ARRAY: {
        QVariantList list;
        for (int i = 0; i < node.u.list->num; i++)
            list.append(mpvNodeToVariant(node.u.list->values[i]));
        return list;
    }
    case MPV_FORMAT_NODE_MAP: {
        QVariantMap map;
        for (int i = 0; i < node.u.list->num; i++) {
            QString key = QString::fromUtf8(node.u.list->keys[i]);
            map[key] = mpvNodeToVariant(node.u.list->values[i]);
        }
        return map;
    }
    default:
        return QVariant();
    }
}

// Helper: free mpv_node
static void freeMpvNode(mpv_node &node)
{
    mpv_free_node_contents(&node);
}

MpvEngine::MpvEngine(QObject *parent)
    : QObject(parent)
{
    initMpv();
}

MpvEngine::~MpvEngine()
{
    if (m_mpv) {
        mpv_terminate_destroy(m_mpv);
        m_mpv = nullptr;
    }
}

void MpvEngine::initMpv()
{
    m_mpv = mpv_create();
    if (!m_mpv) {
        qFatal("Failed to create mpv instance");
        return;
    }

    // Default options for a good playback experience
    mpv_set_option_string(m_mpv, "terminal", "no");
    mpv_set_option_string(m_mpv, "msg-level", "all=warn");
    mpv_set_option_string(m_mpv, "vo", "libmpv");
    mpv_set_option_string(m_mpv, "hwdec", "auto-safe");
    mpv_set_option_string(m_mpv, "keep-open", "yes");
    mpv_set_option_string(m_mpv, "idle", "yes");
    mpv_set_option_string(m_mpv, "input-default-bindings", "no");
    mpv_set_option_string(m_mpv, "input-vo-keyboard", "no");
    mpv_set_option_string(m_mpv, "osc", "no");
    mpv_set_option_string(m_mpv, "osd-level", "0");
    mpv_set_option_string(m_mpv, "ytdl", "no");
    mpv_set_option_string(m_mpv, "screenshot-directory",
        QStandardPaths::writableLocation(QStandardPaths::PicturesLocation).toUtf8().constData());

    // Config directory
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(configDir);

    if (mpv_initialize(m_mpv) < 0) {
        qFatal("Failed to initialize mpv");
        return;
    }

    observeProperties();

    // Set up event callback via Qt signal
    mpv_set_wakeup_callback(m_mpv, [](void *ctx) {
        QMetaObject::invokeMethod(static_cast<MpvEngine *>(ctx),
                                  "onMpvEvent", Qt::QueuedConnection);
    }, this);
}

void MpvEngine::observeProperties()
{
    mpv_observe_property(m_mpv, 0, "pause", MPV_FORMAT_FLAG);
    mpv_observe_property(m_mpv, 0, "duration", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 0, "time-pos", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 0, "volume", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 0, "mute", MPV_FORMAT_FLAG);
    mpv_observe_property(m_mpv, 0, "speed", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 0, "media-title", MPV_FORMAT_STRING);
    mpv_observe_property(m_mpv, 0, "path", MPV_FORMAT_STRING);
    mpv_observe_property(m_mpv, 0, "video-params/w", MPV_FORMAT_INT64);
    mpv_observe_property(m_mpv, 0, "video-params/h", MPV_FORMAT_INT64);
    mpv_observe_property(m_mpv, 0, "core-idle", MPV_FORMAT_FLAG);
    mpv_observe_property(m_mpv, 0, "sid", MPV_FORMAT_INT64);
    mpv_observe_property(m_mpv, 0, "aid", MPV_FORMAT_INT64);
    mpv_observe_property(m_mpv, 0, "track-list", MPV_FORMAT_NODE);
    mpv_observe_property(m_mpv, 0, "playlist-count", MPV_FORMAT_INT64);
    mpv_observe_property(m_mpv, 0, "playlist-pos", MPV_FORMAT_INT64);
    mpv_observe_property(m_mpv, 0, "chapter", MPV_FORMAT_INT64);
    mpv_observe_property(m_mpv, 0, "chapter-list/count", MPV_FORMAT_INT64);
    mpv_observe_property(m_mpv, 0, "sub-delay", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 0, "sub-scale", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 0, "brightness", MPV_FORMAT_INT64);
    mpv_observe_property(m_mpv, 0, "contrast", MPV_FORMAT_INT64);
    mpv_observe_property(m_mpv, 0, "saturation", MPV_FORMAT_INT64);
    mpv_observe_property(m_mpv, 0, "gamma", MPV_FORMAT_INT64);
    mpv_observe_property(m_mpv, 0, "hue", MPV_FORMAT_INT64);
}

void MpvEngine::onMpvEvent()
{
    while (m_mpv) {
        mpv_event *event = mpv_wait_event(m_mpv, 0);
        if (event->event_id == MPV_EVENT_NONE) break;
        processEvent(event);
    }
}

void MpvEngine::processEvent(mpv_event *event)
{
    switch (event->event_id) {
    case MPV_EVENT_PROPERTY_CHANGE: {
        auto *prop = static_cast<mpv_event_property *>(event->data);
        QString name = QString::fromUtf8(prop->name);

        if (name == "pause" && prop->format == MPV_FORMAT_FLAG) {
            bool paused = *static_cast<int *>(prop->data);
            if (m_paused != paused) {
                m_paused = paused;
                m_playing = !paused;
                emit pausedChanged();
                emit playingChanged();
            }
        } else if (name == "duration" && prop->format == MPV_FORMAT_DOUBLE) {
            double d = *static_cast<double *>(prop->data);
            if (m_duration != d) {
                m_duration = d;
                emit durationChanged();
            }
        } else if (name == "time-pos" && prop->format == MPV_FORMAT_DOUBLE) {
            double p = *static_cast<double *>(prop->data);
            if (m_position != p) {
                m_position = p;
                emit positionChanged();
            }
        } else if (name == "volume" && prop->format == MPV_FORMAT_DOUBLE) {
            double v = *static_cast<double *>(prop->data);
            if (m_volume != v) {
                m_volume = v;
                emit volumeChanged();
            }
        } else if (name == "mute" && prop->format == MPV_FORMAT_FLAG) {
            bool m = *static_cast<int *>(prop->data);
            if (m_muted != m) {
                m_muted = m;
                emit mutedChanged();
            }
        } else if (name == "speed" && prop->format == MPV_FORMAT_DOUBLE) {
            double s = *static_cast<double *>(prop->data);
            if (m_speed != s) {
                m_speed = s;
                emit speedChanged();
            }
        } else if (name == "media-title" && prop->format == MPV_FORMAT_STRING) {
            QString t = QString::fromUtf8(*static_cast<char **>(prop->data));
            if (m_mediaTitle != t) {
                m_mediaTitle = t;
                emit mediaTitleChanged();
            }
        } else if (name == "path" && prop->format == MPV_FORMAT_STRING) {
            QString p = QString::fromUtf8(*static_cast<char **>(prop->data));
            if (m_filePath != p) {
                m_filePath = p;
                emit filePathChanged();
            }
        } else if (name == "video-params/w" && prop->format == MPV_FORMAT_INT64) {
            int w = static_cast<int>(*static_cast<int64_t *>(prop->data));
            if (m_videoSize.width() != w) {
                m_videoSize.setWidth(w);
                emit videoSizeChanged();
            }
        } else if (name == "video-params/h" && prop->format == MPV_FORMAT_INT64) {
            int h = static_cast<int>(*static_cast<int64_t *>(prop->data));
            if (m_videoSize.height() != h) {
                m_videoSize.setHeight(h);
                emit videoSizeChanged();
            }
        } else if (name == "core-idle" && prop->format == MPV_FORMAT_FLAG) {
            bool idle = *static_cast<int *>(prop->data);
            if (m_idle != idle) {
                m_idle = idle;
                emit idleChanged();
            }
        } else if (name == "sid" && prop->format == MPV_FORMAT_INT64) {
            int sid = static_cast<int>(*static_cast<int64_t *>(prop->data));
            if (m_subtitleTrack != sid) {
                m_subtitleTrack = sid;
                emit subtitleTrackChanged();
            }
        } else if (name == "aid" && prop->format == MPV_FORMAT_INT64) {
            int aid = static_cast<int>(*static_cast<int64_t *>(prop->data));
            if (m_audioTrack != aid) {
                m_audioTrack = aid;
                emit audioTrackChanged();
            }
        } else if (name == "track-list" && prop->format == MPV_FORMAT_NODE) {
            auto *node = static_cast<mpv_node *>(prop->data);
            QVariant trackVar = mpvNodeToVariant(*node);
            QVariantList tracks = trackVar.toList();

            m_subtitleTracks.clear();
            m_audioTracks.clear();
            m_videoTracks.clear();

            for (const auto &t : tracks) {
                QVariantMap track = t.toMap();
                QString type = track["type"].toString();
                if (type == "sub") m_subtitleTracks.append(track);
                else if (type == "audio") m_audioTracks.append(track);
                else if (type == "video") m_videoTracks.append(track);
            }
            emit tracksChanged();
        } else if (name == "playlist-count" && prop->format == MPV_FORMAT_INT64) {
            int c = static_cast<int>(*static_cast<int64_t *>(prop->data));
            if (m_playlistCount != c) {
                m_playlistCount = c;
                emit playlistChanged();
            }
        } else if (name == "playlist-pos" && prop->format == MPV_FORMAT_INT64) {
            int p = static_cast<int>(*static_cast<int64_t *>(prop->data));
            if (m_playlistPos != p) {
                m_playlistPos = p;
                emit playlistPosChanged();
            }
        } else if (name == "chapter" && prop->format == MPV_FORMAT_INT64) {
            int ch = static_cast<int>(*static_cast<int64_t *>(prop->data));
            if (m_chapter != ch) {
                m_chapter = ch;
                emit chapterChanged();
            }
        } else if (name == "chapter-list/count" && prop->format == MPV_FORMAT_INT64) {
            int c = static_cast<int>(*static_cast<int64_t *>(prop->data));
            if (m_chapterCount != c) {
                m_chapterCount = c;
                emit chapterCountChanged();
            }
        } else if (name == "sub-delay" && prop->format == MPV_FORMAT_DOUBLE) {
            double d = *static_cast<double *>(prop->data);
            if (m_subtitleDelay != d) {
                m_subtitleDelay = d;
                emit subtitleDelayChanged();
            }
        } else if (name == "sub-scale" && prop->format == MPV_FORMAT_DOUBLE) {
            double s = *static_cast<double *>(prop->data);
            if (m_subtitleScale != s) {
                m_subtitleScale = s;
                emit subtitleScaleChanged();
            }
        } else if (name == "brightness" && prop->format == MPV_FORMAT_INT64) {
            double v = static_cast<double>(*static_cast<int64_t *>(prop->data));
            if (m_brightness != v) { m_brightness = v; emit brightnessChanged(); }
        } else if (name == "contrast" && prop->format == MPV_FORMAT_INT64) {
            double v = static_cast<double>(*static_cast<int64_t *>(prop->data));
            if (m_contrast != v) { m_contrast = v; emit contrastChanged(); }
        } else if (name == "saturation" && prop->format == MPV_FORMAT_INT64) {
            double v = static_cast<double>(*static_cast<int64_t *>(prop->data));
            if (m_saturation != v) { m_saturation = v; emit saturationChanged(); }
        } else if (name == "gamma" && prop->format == MPV_FORMAT_INT64) {
            double v = static_cast<double>(*static_cast<int64_t *>(prop->data));
            if (m_gamma != v) { m_gamma = v; emit gammaChanged(); }
        } else if (name == "hue" && prop->format == MPV_FORMAT_INT64) {
            double v = static_cast<double>(*static_cast<int64_t *>(prop->data));
            if (m_hue != v) { m_hue = v; emit hueChanged(); }
        }
        break;
    }
    case MPV_EVENT_START_FILE:
        m_playing = true;
        emit fileStarted();
        emit playingChanged();
        break;
    case MPV_EVENT_END_FILE:
        emit fileEnded();
        break;
    case MPV_EVENT_FILE_LOADED:
        emit fileLoaded();
        break;
    default:
        break;
    }
}

// Property getters
bool MpvEngine::isPlaying() const { return m_playing; }
bool MpvEngine::isPaused() const { return m_paused; }
double MpvEngine::duration() const { return m_duration; }
double MpvEngine::position() const { return m_position; }
double MpvEngine::volume() const { return m_volume; }
bool MpvEngine::isMuted() const { return m_muted; }
double MpvEngine::speed() const { return m_speed; }
QString MpvEngine::mediaTitle() const { return m_mediaTitle; }
QString MpvEngine::filePath() const { return m_filePath; }
QSize MpvEngine::videoSize() const { return m_videoSize; }
bool MpvEngine::isIdle() const { return m_idle; }
int MpvEngine::subtitleTrack() const { return m_subtitleTrack; }
int MpvEngine::audioTrack() const { return m_audioTrack; }
QVariantList MpvEngine::subtitleTracks() const { return m_subtitleTracks; }
QVariantList MpvEngine::audioTracks() const { return m_audioTracks; }
QVariantList MpvEngine::videoTracks() const { return m_videoTracks; }
int MpvEngine::playlistCount() const { return m_playlistCount; }
int MpvEngine::playlistPos() const { return m_playlistPos; }
int MpvEngine::chapter() const { return m_chapter; }
int MpvEngine::chapterCount() const { return m_chapterCount; }
double MpvEngine::subtitleDelay() const { return m_subtitleDelay; }
double MpvEngine::subtitleScale() const { return m_subtitleScale; }
double MpvEngine::brightness() const { return m_brightness; }
double MpvEngine::contrast() const { return m_contrast; }
double MpvEngine::saturation() const { return m_saturation; }
double MpvEngine::gamma() const { return m_gamma; }
double MpvEngine::hue() const { return m_hue; }
QString MpvEngine::screenshotFormat() const { return m_screenshotFormat; }
QString MpvEngine::screenshotDirectory() const { return m_screenshotDir; }

// Property setters
void MpvEngine::setVolume(double vol)
{
    if (m_mpv) {
        double v = qBound(0.0, vol, 150.0);
        mpv_set_property(m_mpv, "volume", MPV_FORMAT_DOUBLE, &v);
    }
}

void MpvEngine::setMuted(bool muted)
{
    if (m_mpv) {
        int flag = muted ? 1 : 0;
        mpv_set_property(m_mpv, "mute", MPV_FORMAT_FLAG, &flag);
    }
}

void MpvEngine::setSpeed(double speed)
{
    if (m_mpv) {
        double s = qBound(0.25, speed, 4.0);
        mpv_set_property(m_mpv, "speed", MPV_FORMAT_DOUBLE, &s);
    }
}

void MpvEngine::setSubtitleTrack(int id)
{
    if (m_mpv) {
        int64_t sid = id;
        mpv_set_property(m_mpv, "sid", MPV_FORMAT_INT64, &sid);
    }
}

void MpvEngine::setAudioTrack(int id)
{
    if (m_mpv) {
        int64_t aid = id;
        mpv_set_property(m_mpv, "aid", MPV_FORMAT_INT64, &aid);
    }
}

void MpvEngine::setChapter(int ch)
{
    if (m_mpv) {
        int64_t val = ch;
        mpv_set_property(m_mpv, "chapter", MPV_FORMAT_INT64, &val);
    }
}

void MpvEngine::setSubtitleDelay(double delay)
{
    if (m_mpv) {
        mpv_set_property(m_mpv, "sub-delay", MPV_FORMAT_DOUBLE, &delay);
    }
}

void MpvEngine::setSubtitleScale(double scale)
{
    if (m_mpv) {
        double s = qBound(0.1, scale, 5.0);
        mpv_set_property(m_mpv, "sub-scale", MPV_FORMAT_DOUBLE, &s);
    }
}

void MpvEngine::setBrightness(double val)
{
    if (m_mpv) {
        int64_t v = static_cast<int64_t>(qBound(-100.0, val, 100.0));
        mpv_set_property(m_mpv, "brightness", MPV_FORMAT_INT64, &v);
    }
}

void MpvEngine::setContrast(double val)
{
    if (m_mpv) {
        int64_t v = static_cast<int64_t>(qBound(-100.0, val, 100.0));
        mpv_set_property(m_mpv, "contrast", MPV_FORMAT_INT64, &v);
    }
}

void MpvEngine::setSaturation(double val)
{
    if (m_mpv) {
        int64_t v = static_cast<int64_t>(qBound(-100.0, val, 100.0));
        mpv_set_property(m_mpv, "saturation", MPV_FORMAT_INT64, &v);
    }
}

void MpvEngine::setGamma(double val)
{
    if (m_mpv) {
        int64_t v = static_cast<int64_t>(qBound(-100.0, val, 100.0));
        mpv_set_property(m_mpv, "gamma", MPV_FORMAT_INT64, &v);
    }
}

void MpvEngine::setHue(double val)
{
    if (m_mpv) {
        int64_t v = static_cast<int64_t>(qBound(-100.0, val, 100.0));
        mpv_set_property(m_mpv, "hue", MPV_FORMAT_INT64, &v);
    }
}

void MpvEngine::setScreenshotFormat(const QString &fmt)
{
    if (m_mpv) {
        m_screenshotFormat = fmt;
        mpv_set_property_string(m_mpv, "screenshot-format", fmt.toUtf8().constData());
        emit screenshotFormatChanged();
    }
}

void MpvEngine::setScreenshotDirectory(const QString &dir)
{
    if (m_mpv) {
        m_screenshotDir = dir;
        mpv_set_property_string(m_mpv, "screenshot-directory", dir.toUtf8().constData());
        emit screenshotDirectoryChanged();
    }
}

// Commands
void MpvEngine::loadFile(const QString &path)
{
    if (!m_mpv) return;
    QByteArray utf8 = path.toUtf8();
    const char *args[] = {"loadfile", utf8.constData(), nullptr};
    mpv_command_async(m_mpv, 0, args);
}

void MpvEngine::loadUrl(const QString &url)
{
    loadFile(url); // mpv handles URLs natively
}

void MpvEngine::togglePause()
{
    if (!m_mpv) return;
    const char *args[] = {"cycle", "pause", nullptr};
    mpv_command_async(m_mpv, 0, args);
}

void MpvEngine::pause()
{
    if (m_mpv) {
        int flag = 1;
        mpv_set_property(m_mpv, "pause", MPV_FORMAT_FLAG, &flag);
    }
}

void MpvEngine::resume()
{
    if (m_mpv) {
        int flag = 0;
        mpv_set_property(m_mpv, "pause", MPV_FORMAT_FLAG, &flag);
    }
}

void MpvEngine::stop()
{
    if (!m_mpv) return;
    const char *args[] = {"stop", nullptr};
    mpv_command_async(m_mpv, 0, args);
}

void MpvEngine::seek(double seconds)
{
    if (!m_mpv) return;
    QByteArray val = QByteArray::number(seconds);
    const char *args[] = {"seek", val.constData(), "relative", nullptr};
    mpv_command_async(m_mpv, 0, args);
}

void MpvEngine::seekAbsolute(double seconds)
{
    if (!m_mpv) return;
    QByteArray val = QByteArray::number(seconds);
    const char *args[] = {"seek", val.constData(), "absolute", nullptr};
    mpv_command_async(m_mpv, 0, args);
}

void MpvEngine::seekPercent(double percent)
{
    if (!m_mpv) return;
    QByteArray val = QByteArray::number(percent);
    const char *args[] = {"seek", val.constData(), "absolute-percent", nullptr};
    mpv_command_async(m_mpv, 0, args);
}

void MpvEngine::frameStep()
{
    if (!m_mpv) return;
    const char *args[] = {"frame-step", nullptr};
    mpv_command_async(m_mpv, 0, args);
}

void MpvEngine::frameBackStep()
{
    if (!m_mpv) return;
    const char *args[] = {"frame-back-step", nullptr};
    mpv_command_async(m_mpv, 0, args);
}

void MpvEngine::addSubtitleFile(const QString &path)
{
    if (!m_mpv) return;
    QByteArray utf8 = path.toUtf8();
    const char *args[] = {"sub-add", utf8.constData(), "auto", nullptr};
    mpv_command_async(m_mpv, 0, args);
}

void MpvEngine::playlistAppend(const QString &path)
{
    if (!m_mpv) return;
    QByteArray utf8 = path.toUtf8();
    const char *args[] = {"loadfile", utf8.constData(), "append-play", nullptr};
    mpv_command_async(m_mpv, 0, args);
}

void MpvEngine::playlistRemove(int index)
{
    if (!m_mpv) return;
    QByteArray idx = QByteArray::number(index);
    const char *args[] = {"playlist-remove", idx.constData(), nullptr};
    mpv_command_async(m_mpv, 0, args);
}

void MpvEngine::playlistMove(int from, int to)
{
    if (!m_mpv) return;
    QByteArray f = QByteArray::number(from);
    QByteArray t = QByteArray::number(to);
    const char *args[] = {"playlist-move", f.constData(), t.constData(), nullptr};
    mpv_command_async(m_mpv, 0, args);
}

void MpvEngine::playlistPlayIndex(int index)
{
    if (!m_mpv) return;
    int64_t pos = index;
    mpv_set_property(m_mpv, "playlist-pos", MPV_FORMAT_INT64, &pos);
}

void MpvEngine::playlistNext()
{
    if (!m_mpv) return;
    const char *args[] = {"playlist-next", "weak", nullptr};
    mpv_command_async(m_mpv, 0, args);
}

void MpvEngine::playlistPrev()
{
    if (!m_mpv) return;
    const char *args[] = {"playlist-prev", "weak", nullptr};
    mpv_command_async(m_mpv, 0, args);
}

void MpvEngine::playlistClear()
{
    if (!m_mpv) return;
    const char *args[] = {"playlist-clear", nullptr};
    mpv_command_async(m_mpv, 0, args);
}

void MpvEngine::screenshot()
{
    if (!m_mpv) return;
    const char *args[] = {"screenshot", nullptr};
    mpv_command_async(m_mpv, 0, args);
    emit screenshotTaken(m_screenshotDir);
}

void MpvEngine::screenshotToClipboard()
{
    if (!m_mpv) return;
    const char *args[] = {"screenshot", "video", nullptr};
    mpv_command_async(m_mpv, 0, args);
    emit screenshotTaken("");
}

void MpvEngine::cycleSubtitleTrack()
{
    if (!m_mpv) return;
    const char *args[] = {"cycle", "sub", nullptr};
    mpv_command_async(m_mpv, 0, args);
}

void MpvEngine::setSubtitleFont(const QString &font)
{
    if (m_mpv) {
        mpv_set_property_string(m_mpv, "sub-font", font.toUtf8().constData());
    }
}

void MpvEngine::setSubtitleColor(const QString &color)
{
    if (m_mpv) {
        mpv_set_property_string(m_mpv, "sub-color", color.toUtf8().constData());
    }
}

void MpvEngine::setSubtitleBorderSize(double size)
{
    if (m_mpv) {
        mpv_set_property(m_mpv, "sub-border-size", MPV_FORMAT_DOUBLE, &size);
    }
}

void MpvEngine::setSubtitlePosition(int pos)
{
    if (m_mpv) {
        int64_t p = pos;
        mpv_set_property(m_mpv, "sub-pos", MPV_FORMAT_INT64, &p);
    }
}

void MpvEngine::resetFilters()
{
    setBrightness(0);
    setContrast(0);
    setSaturation(0);
    setGamma(0);
    setHue(0);
}

void MpvEngine::setOption(const QString &name, const QString &value)
{
    if (!m_mpv) return;
    mpv_set_option_string(m_mpv, name.toUtf8().constData(), value.toUtf8().constData());
}

QVariant MpvEngine::getProperty(const QString &name) const
{
    if (!m_mpv) return QVariant();
    mpv_node node;
    int err = mpv_get_property(m_mpv, name.toUtf8().constData(), MPV_FORMAT_NODE, &node);
    if (err < 0) return QVariant();
    QVariant result = mpvNodeToVariant(node);
    freeMpvNode(node);
    return result;
}

void MpvEngine::setProperty(const QString &name, const QVariant &value)
{
    if (!m_mpv) return;

    if (value.typeId() == QMetaType::Bool) {
        int flag = value.toBool() ? 1 : 0;
        mpv_set_property(m_mpv, name.toUtf8().constData(), MPV_FORMAT_FLAG, &flag);
    } else if (value.typeId() == QMetaType::Int || value.typeId() == QMetaType::LongLong) {
        int64_t val = value.toLongLong();
        mpv_set_property(m_mpv, name.toUtf8().constData(), MPV_FORMAT_INT64, &val);
    } else if (value.typeId() == QMetaType::Double || value.typeId() == QMetaType::Float) {
        double val = value.toDouble();
        mpv_set_property(m_mpv, name.toUtf8().constData(), MPV_FORMAT_DOUBLE, &val);
    } else {
        QByteArray val = value.toString().toUtf8();
        mpv_set_property_string(m_mpv, name.toUtf8().constData(), val.constData());
    }
}

void MpvEngine::command(const QStringList &args)
{
    if (!m_mpv || args.isEmpty()) return;
    QVector<QByteArray> utf8Args;
    QVector<const char *> cArgs;
    for (const auto &a : args) {
        utf8Args.append(a.toUtf8());
        cArgs.append(utf8Args.last().constData());
    }
    cArgs.append(nullptr);
    mpv_command_async(m_mpv, 0, cArgs.data());
}
