#ifndef CONFIG_H
#define CONFIG_H

#include <QSettings>
#include <QVariant>
#include <QString>

class Config
{
public:
    static Config &instance();

    QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;
    void setValue(const QString &key, const QVariant &value);
    void sync();

    // Commonly accessed settings
    double volume() const;
    void setVolume(double vol);
    bool isMuted() const;
    void setMuted(bool muted);
    QString lastOpenDir() const;
    void setLastOpenDir(const QString &dir);
    QString hwdec() const;
    void setHwdec(const QString &mode);

private:
    Config();
    QSettings m_settings;
};

#endif // CONFIG_H
