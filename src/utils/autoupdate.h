// autoupdate.h - WinSparkle auto-update integration for IINA Windows
// Wraps WinSparkle library for automatic application updates

#ifndef AUTOUPDATE_H
#define AUTOUPDATE_H

#include <QObject>
#include <QString>

/**
 * @brief Auto-update manager using WinSparkle
 *
 * Integrates WinSparkle for Sparkle-compatible auto-updates on Windows.
 * Provides a Qt-friendly wrapper around the WinSparkle C API.
 *
 * Usage:
 *   AutoUpdate::instance().initialize("https://iina.io/appcast.xml");
 *   AutoUpdate::instance().checkForUpdates(); // Manual check
 *
 * WinSparkle automatically checks for updates on startup and periodically.
 */
class AutoUpdate : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Get singleton instance
     */
    static AutoUpdate& instance();

    /**
     * @brief Initialize WinSparkle with appcast URL
     * @param appcastUrl URL to the Sparkle appcast XML feed
     * @param automaticChecks Enable automatic periodic checks (default: true)
     *
     * Call this once at application startup, before any update operations.
     */
    void initialize(const QString& appcastUrl, bool automaticChecks = true);

    /**
     * @brief Check for updates manually
     *
     * Shows UI if an update is available. Does nothing if already initialized
     * to check automatically.
     */
    void checkForUpdates();

    /**
     * @brief Check for updates in background without UI
     *
     * Silently checks for updates. If found, shows a subtle notification.
     */
    void checkForUpdatesBackground();

    /**
     * @brief Set update check interval
     * @param seconds Interval in seconds (default: 86400 = 24 hours)
     */
    void setCheckInterval(int seconds);

    /**
     * @brief Enable or disable automatic update checks
     * @param enabled True to enable automatic checks
     */
    void setAutomaticChecks(bool enabled);

    /**
     * @brief Get the current application version
     * @return Version string (e.g., "0.1.0")
     */
    QString currentVersion() const;

    /**
     * @brief Cleanup WinSparkle on shutdown
     *
     * Call this before application exit. Automatically called by destructor.
     */
    void shutdown();

    /**
     * @brief Check if auto-update is supported on this platform
     * @return True on Windows, false otherwise
     */
    static bool isSupported();

signals:
    /**
     * @brief Emitted when an update check is started
     */
    void updateCheckStarted();

    /**
     * @brief Emitted when an update is found
     * @param version New version string
     */
    void updateAvailable(const QString& version);

    /**
     * @brief Emitted when no update is available
     */
    void noUpdateAvailable();

    /**
     * @brief Emitted when update check fails
     * @param error Error message
     */
    void updateCheckFailed(const QString& error);

private:
    AutoUpdate();
    ~AutoUpdate();
    AutoUpdate(const AutoUpdate&) = delete;
    AutoUpdate& operator=(const AutoUpdate&) = delete;

    bool m_initialized;
    QString m_appcastUrl;

    // WinSparkle callbacks (static)
    static void onUpdateCheckStarted();
    static void onUpdateCheckFinished();
    static void onUpdateFound();
    static void onUpdateNotFound();
    static void onError();
};

#endif // AUTOUPDATE_H
