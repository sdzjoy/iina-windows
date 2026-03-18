#ifndef MPVENGINE_H
#define MPVENGINE_H

#include <QObject>
#include <QVariant>
#include <QSize>
#include <QString>
#include <QStringList>

struct mpv_handle;
struct mpv_event;

class MpvEngine : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool playing READ isPlaying NOTIFY playingChanged)
    Q_PROPERTY(bool paused READ isPaused NOTIFY pausedChanged)
    Q_PROPERTY(double duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(double position READ position NOTIFY positionChanged)
    Q_PROPERTY(double volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(double speed READ speed WRITE setSpeed NOTIFY speedChanged)
    Q_PROPERTY(QString mediaTitle READ mediaTitle NOTIFY mediaTitleChanged)
    Q_PROPERTY(QString filePath READ filePath NOTIFY filePathChanged)
    Q_PROPERTY(QSize videoSize READ videoSize NOTIFY videoSizeChanged)
    Q_PROPERTY(bool idle READ isIdle NOTIFY idleChanged)
    Q_PROPERTY(int subtitleTrack READ subtitleTrack WRITE setSubtitleTrack NOTIFY subtitleTrackChanged)
    Q_PROPERTY(int audioTrack READ audioTrack WRITE setAudioTrack NOTIFY audioTrackChanged)
    Q_PROPERTY(QVariantList subtitleTracks READ subtitleTracks NOTIFY tracksChanged)
    Q_PROPERTY(QVariantList audioTracks READ audioTracks NOTIFY tracksChanged)
    Q_PROPERTY(QVariantList videoTracks READ videoTracks NOTIFY tracksChanged)
    Q_PROPERTY(int playlistCount READ playlistCount NOTIFY playlistChanged)
    Q_PROPERTY(int playlistPos READ playlistPos NOTIFY playlistPosChanged)
    Q_PROPERTY(int chapter READ chapter WRITE setChapter NOTIFY chapterChanged)
    Q_PROPERTY(int chapterCount READ chapterCount NOTIFY chapterCountChanged)
    Q_PROPERTY(double subtitleDelay READ subtitleDelay WRITE setSubtitleDelay NOTIFY subtitleDelayChanged)
    Q_PROPERTY(double subtitleScale READ subtitleScale WRITE setSubtitleScale NOTIFY subtitleScaleChanged)
    Q_PROPERTY(double brightness READ brightness WRITE setBrightness NOTIFY brightnessChanged)
    Q_PROPERTY(double contrast READ contrast WRITE setContrast NOTIFY contrastChanged)
    Q_PROPERTY(double saturation READ saturation WRITE setSaturation NOTIFY saturationChanged)
    Q_PROPERTY(double gamma READ gamma WRITE setGamma NOTIFY gammaChanged)
    Q_PROPERTY(double hue READ hue WRITE setHue NOTIFY hueChanged)
    Q_PROPERTY(QString screenshotFormat READ screenshotFormat WRITE setScreenshotFormat NOTIFY screenshotFormatChanged)
    Q_PROPERTY(QString screenshotDirectory READ screenshotDirectory WRITE setScreenshotDirectory NOTIFY screenshotDirectoryChanged)

public:
    explicit MpvEngine(QObject *parent = nullptr);
    ~MpvEngine() override;

    mpv_handle *handle() const { return m_mpv; }

    bool isPlaying() const;
    bool isPaused() const;
    double duration() const;
    double position() const;
    double volume() const;
    bool isMuted() const;
    double speed() const;
    QString mediaTitle() const;
    QString filePath() const;
    QSize videoSize() const;
    bool isIdle() const;
    int subtitleTrack() const;
    int audioTrack() const;
    QVariantList subtitleTracks() const;
    QVariantList audioTracks() const;
    QVariantList videoTracks() const;
    int playlistCount() const;
    int playlistPos() const;
    int chapter() const;
    int chapterCount() const;
    double subtitleDelay() const;
    double subtitleScale() const;
    double brightness() const;
    double contrast() const;
    double saturation() const;
    double gamma() const;
    double hue() const;
    QString screenshotFormat() const;
    QString screenshotDirectory() const;

    void setVolume(double vol);
    void setMuted(bool muted);
    void setSpeed(double speed);
    void setSubtitleTrack(int id);
    void setAudioTrack(int id);
    void setChapter(int ch);
    void setSubtitleDelay(double delay);
    void setSubtitleScale(double scale);
    void setBrightness(double val);
    void setContrast(double val);
    void setSaturation(double val);
    void setGamma(double val);
    void setHue(double val);
    void setScreenshotFormat(const QString &fmt);
    void setScreenshotDirectory(const QString &dir);

    Q_INVOKABLE void loadFile(const QString &path);
    Q_INVOKABLE void loadUrl(const QString &url);
    Q_INVOKABLE void togglePause();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void resume();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void seek(double seconds);
    Q_INVOKABLE void seekAbsolute(double seconds);
    Q_INVOKABLE void seekPercent(double percent);
    Q_INVOKABLE void frameStep();
    Q_INVOKABLE void frameBackStep();

    Q_INVOKABLE void addSubtitleFile(const QString &path);
    Q_INVOKABLE void playlistAppend(const QString &path);
    Q_INVOKABLE void playlistRemove(int index);
    Q_INVOKABLE void playlistMove(int from, int to);
    Q_INVOKABLE void playlistPlayIndex(int index);
    Q_INVOKABLE void playlistNext();
    Q_INVOKABLE void playlistPrev();
    Q_INVOKABLE void playlistClear();

    Q_INVOKABLE void screenshot();
    Q_INVOKABLE void screenshotToClipboard();
    Q_INVOKABLE void cycleSubtitleTrack();
    Q_INVOKABLE void setSubtitleFont(const QString &font);
    Q_INVOKABLE void setSubtitleColor(const QString &color);
    Q_INVOKABLE void setSubtitleBorderSize(double size);
    Q_INVOKABLE void setSubtitlePosition(int pos);
    Q_INVOKABLE void resetFilters();
    Q_INVOKABLE void setOption(const QString &name, const QString &value);
    Q_INVOKABLE QVariant getProperty(const QString &name) const;
    Q_INVOKABLE void setProperty(const QString &name, const QVariant &value);
    Q_INVOKABLE void command(const QStringList &args);

signals:
    void playingChanged();
    void pausedChanged();
    void durationChanged();
    void positionChanged();
    void volumeChanged();
    void mutedChanged();
    void speedChanged();
    void mediaTitleChanged();
    void filePathChanged();
    void videoSizeChanged();
    void idleChanged();
    void subtitleTrackChanged();
    void audioTrackChanged();
    void tracksChanged();
    void playlistChanged();
    void playlistPosChanged();
    void chapterChanged();
    void chapterCountChanged();
    void subtitleDelayChanged();
    void subtitleScaleChanged();
    void brightnessChanged();
    void contrastChanged();
    void saturationChanged();
    void gammaChanged();
    void hueChanged();
    void screenshotFormatChanged();
    void screenshotDirectoryChanged();
    void screenshotTaken(const QString &path);
    void fileStarted();
    void fileEnded();
    void fileLoaded();
    void mpvEvent();

private slots:
    void onMpvEvent();

private:
    void initMpv();
    void processEvent(mpv_event *event);
    void observeProperties();

    mpv_handle *m_mpv = nullptr;

    bool m_playing = false;
    bool m_paused = false;
    double m_duration = 0.0;
    double m_position = 0.0;
    double m_volume = 100.0;
    bool m_muted = false;
    double m_speed = 1.0;
    QString m_mediaTitle;
    QString m_filePath;
    QSize m_videoSize;
    bool m_idle = true;
    int m_subtitleTrack = 0;
    int m_audioTrack = 0;
    QVariantList m_subtitleTracks;
    QVariantList m_audioTracks;
    QVariantList m_videoTracks;
    int m_playlistCount = 0;
    int m_playlistPos = -1;
    int m_chapter = -1;
    int m_chapterCount = 0;
    double m_subtitleDelay = 0.0;
    double m_subtitleScale = 1.0;
    double m_brightness = 0.0;
    double m_contrast = 0.0;
    double m_saturation = 0.0;
    double m_gamma = 0.0;
    double m_hue = 0.0;
    QString m_screenshotFormat = "png";
    QString m_screenshotDir;
};

#endif // MPVENGINE_H
