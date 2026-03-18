#include "shortcutmanager.h"

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

ShortcutManager::ShortcutManager(QObject *parent)
    : QObject(parent)
{
    initDefaults();
    load();
}

void ShortcutManager::initDefaults()
{
    auto add = [this](const QString &action, const QString &label,
                      const QString &category, const QString &key) {
        Shortcut s;
        s.action = action;
        s.label = label;
        s.category = category;
        s.defaultKey = key;
        s.currentKey = key;
        m_actionIndex[action] = m_shortcuts.size();
        m_shortcuts.append(s);
    };

    // Playback
    add("play_pause",     tr("Play / Pause"),       tr("Playback"), "Space");
    add("stop",           tr("Stop"),               tr("Playback"), "");
    add("playlist_next",  tr("Next in Playlist"),   tr("Playback"), "");
    add("playlist_prev",  tr("Previous in Playlist"),tr("Playback"), "");
    add("frame_step",     tr("Frame Step Forward"),  tr("Playback"), ".");
    add("frame_back",     tr("Frame Step Backward"), tr("Playback"), ",");
    add("speed_up",       tr("Speed +0.25x"),       tr("Playback"), "]");
    add("speed_down",     tr("Speed -0.25x"),       tr("Playback"), "[");
    add("speed_reset",    tr("Reset Speed"),        tr("Playback"), "Backspace");
    add("chapter_next",   tr("Next Chapter"),       tr("Playback"), "Shift+Right");
    add("chapter_prev",   tr("Previous Chapter"),   tr("Playback"), "Shift+Left");

    // Seek
    add("seek_forward_5",  tr("Seek Forward 5s"),   tr("Seek"), "Right");
    add("seek_backward_5", tr("Seek Backward 5s"),  tr("Seek"), "Left");
    add("seek_forward_30", tr("Seek Forward 30s"),  tr("Seek"), "Ctrl+Right");
    add("seek_backward_30",tr("Seek Backward 30s"), tr("Seek"), "Ctrl+Left");
    add("seek_forward_60", tr("Seek Forward 60s"),  tr("Seek"), "Shift+Up");
    add("seek_backward_60",tr("Seek Backward 60s"), tr("Seek"), "Shift+Down");

    // Volume
    add("volume_up",      tr("Volume Up"),          tr("Volume"), "Up");
    add("volume_down",    tr("Volume Down"),        tr("Volume"), "Down");
    add("mute_toggle",    tr("Toggle Mute"),        tr("Volume"), "M");

    // Subtitle
    add("sub_load",       tr("Load Subtitle File"), tr("Subtitle"), "");
    add("sub_cycle",      tr("Cycle Subtitle Track"),tr("Subtitle"), "S");
    add("sub_delay_inc",  tr("Subtitle Delay +0.5s"),tr("Subtitle"), "Z");
    add("sub_delay_dec",  tr("Subtitle Delay -0.5s"),tr("Subtitle"), "Shift+Z");
    add("sub_scale_up",   tr("Subtitle Size +"),    tr("Subtitle"), "Shift+=");
    add("sub_scale_down", tr("Subtitle Size -"),    tr("Subtitle"), "Shift+-");

    // Window
    add("fullscreen",     tr("Toggle Fullscreen"),  tr("Window"), "F");
    add("exit_fullscreen",tr("Exit Fullscreen"),    tr("Window"), "Escape");
    add("playlist_toggle",tr("Toggle Playlist"),    tr("Window"), "P");
    add("screenshot",     tr("Screenshot"),         tr("Window"), "Ctrl+S");
    add("open_file",      tr("Open File"),          tr("Window"), "Ctrl+O");
    add("open_url",       tr("Open URL"),           tr("Window"), "Ctrl+Shift+O");
    add("quit",           tr("Quit"),               tr("Window"), "Ctrl+Q");
    add("settings",       tr("Settings"),           tr("Window"), "Ctrl+,");
}

QVariantList ShortcutManager::shortcuts() const
{
    QVariantList list;
    for (const auto &s : m_shortcuts) {
        QVariantMap m;
        m["action"] = s.action;
        m["label"] = s.label;
        m["category"] = s.category;
        m["defaultKey"] = s.defaultKey;
        m["currentKey"] = s.currentKey;
        m["isModified"] = (s.currentKey != s.defaultKey);
        list.append(m);
    }
    return list;
}

QString ShortcutManager::keyForAction(const QString &action) const
{
    auto it = m_actionIndex.find(action);
    if (it != m_actionIndex.end())
        return m_shortcuts[it.value()].currentKey;
    return {};
}

void ShortcutManager::setKey(const QString &action, const QString &key)
{
    auto it = m_actionIndex.find(action);
    if (it == m_actionIndex.end()) return;

    m_shortcuts[it.value()].currentKey = key;
    save();
    emit shortcutChanged(action, key);
    emit shortcutsChanged();
}

void ShortcutManager::resetToDefault(const QString &action)
{
    auto it = m_actionIndex.find(action);
    if (it == m_actionIndex.end()) return;

    auto &s = m_shortcuts[it.value()];
    s.currentKey = s.defaultKey;
    save();
    emit shortcutChanged(action, s.currentKey);
    emit shortcutsChanged();
}

void ShortcutManager::resetAllToDefaults()
{
    for (auto &s : m_shortcuts) {
        s.currentKey = s.defaultKey;
    }
    save();
    emit shortcutsChanged();
}

QVariantList ShortcutManager::shortcutsByCategory(const QString &category) const
{
    QVariantList list;
    for (const auto &s : m_shortcuts) {
        if (s.category == category) {
            QVariantMap m;
            m["action"] = s.action;
            m["label"] = s.label;
            m["category"] = s.category;
            m["defaultKey"] = s.defaultKey;
            m["currentKey"] = s.currentKey;
            m["isModified"] = (s.currentKey != s.defaultKey);
            list.append(m);
        }
    }
    return list;
}

QStringList ShortcutManager::categories() const
{
    QStringList cats;
    for (const auto &s : m_shortcuts) {
        if (!cats.contains(s.category))
            cats.append(s.category);
    }
    return cats;
}

void ShortcutManager::load()
{
    QFile file(configFilePath());
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject obj = doc.object();

    for (auto it = obj.begin(); it != obj.end(); ++it) {
        auto idx = m_actionIndex.find(it.key());
        if (idx != m_actionIndex.end()) {
            m_shortcuts[idx.value()].currentKey = it.value().toString();
        }
    }
}

void ShortcutManager::save() const
{
    QJsonObject obj;
    for (const auto &s : m_shortcuts) {
        if (s.currentKey != s.defaultKey) {
            obj[s.action] = s.currentKey;
        }
    }

    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);

    QFile file(configFilePath());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(obj).toJson(QJsonDocument::Compact));
    }
}

QString ShortcutManager::configFilePath() const
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
           + "/shortcuts.json";
}
