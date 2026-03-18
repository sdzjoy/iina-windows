#include "config.h"

Config &Config::instance()
{
    static Config config;
    return config;
}

Config::Config()
    : m_settings(QSettings::IniFormat, QSettings::UserScope, "IINA", "IINA")
{
}

QVariant Config::value(const QString &key, const QVariant &defaultValue) const
{
    return m_settings.value(key, defaultValue);
}

void Config::setValue(const QString &key, const QVariant &value)
{
    m_settings.setValue(key, value);
}

void Config::sync()
{
    m_settings.sync();
}

double Config::volume() const
{
    return m_settings.value("player/volume", 100.0).toDouble();
}

void Config::setVolume(double vol)
{
    m_settings.setValue("player/volume", vol);
}

bool Config::isMuted() const
{
    return m_settings.value("player/muted", false).toBool();
}

void Config::setMuted(bool muted)
{
    m_settings.setValue("player/muted", muted);
}

QString Config::lastOpenDir() const
{
    return m_settings.value("general/lastOpenDir", "").toString();
}

void Config::setLastOpenDir(const QString &dir)
{
    m_settings.setValue("general/lastOpenDir", dir);
}

QString Config::hwdec() const
{
    return m_settings.value("player/hwdec", "auto-safe").toString();
}

void Config::setHwdec(const QString &mode)
{
    m_settings.setValue("player/hwdec", mode);
}
