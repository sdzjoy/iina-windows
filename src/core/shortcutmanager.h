#ifndef SHORTCUTMANAGER_H
#define SHORTCUTMANAGER_H

#include <QObject>
#include <QVariantList>
#include <QMap>
#include <QJsonArray>

class ShortcutManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList shortcuts READ shortcuts NOTIFY shortcutsChanged)

public:
    struct Shortcut {
        QString action;       // Internal action ID, e.g. "play_pause"
        QString label;        // Display label, e.g. "Play / Pause"
        QString category;     // "Playback", "Seek", "Volume", "Subtitle", "Window"
        QString defaultKey;   // Default key sequence
        QString currentKey;   // User-configured key sequence
    };

    explicit ShortcutManager(QObject *parent = nullptr);

    QVariantList shortcuts() const;

    Q_INVOKABLE QString keyForAction(const QString &action) const;
    Q_INVOKABLE void setKey(const QString &action, const QString &key);
    Q_INVOKABLE void resetToDefault(const QString &action);
    Q_INVOKABLE void resetAllToDefaults();
    Q_INVOKABLE QVariantList shortcutsByCategory(const QString &category) const;
    Q_INVOKABLE QStringList categories() const;

signals:
    void shortcutsChanged();
    void shortcutChanged(const QString &action, const QString &newKey);

private:
    void initDefaults();
    void load();
    void save() const;
    QString configFilePath() const;

    QVector<Shortcut> m_shortcuts;
    QMap<QString, int> m_actionIndex; // action -> index in m_shortcuts
};

#endif // SHORTCUTMANAGER_H
