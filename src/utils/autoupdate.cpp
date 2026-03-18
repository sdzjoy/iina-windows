// autoupdate.cpp - WinSparkle auto-update implementation

#include "autoupdate.h"
#include "logger.h"

#include <QCoreApplication>
#include <QVersionNumber>

#ifdef Q_OS_WIN
#include <winsparkle.h>
#endif

// Static instance pointer for callbacks
static AutoUpdate* s_instance = nullptr;

AutoUpdate& AutoUpdate::instance()
{
    static AutoUpdate instance;
    return instance;
}

AutoUpdate::AutoUpdate()
    : QObject(nullptr)
    , m_initialized(false)
{
    s_instance = this;
}

AutoUpdate::~AutoUpdate()
{
    shutdown();
    s_instance = nullptr;
}

void AutoUpdate::initialize(const QString& appcastUrl, bool automaticChecks)
{
#ifdef Q_OS_WIN
    if (m_initialized) {
        qWarning() << "AutoUpdate already initialized";
        return;
    }

    m_appcastUrl = appcastUrl;

    qInfo() << "Initializing WinSparkle auto-update";
    qInfo() << "  Appcast URL:" << appcastUrl;
    qInfo() << "  Automatic checks:" << automaticChecks;

    // Set application metadata
    win_sparkle_set_app_details(
        L"IINA Contributors",  // Company name
        L"IINA",               // App name
        currentVersion().toStdWString().c_str()  // Version
    );

    // Set appcast URL
    win_sparkle_set_appcast_url(appcastUrl.toStdString().c_str());

    // Set update check interval (24 hours by default)
    win_sparkle_set_update_check_interval(86400);

    // Enable/disable automatic checks
    win_sparkle_set_automatic_check_for_updates(automaticChecks ? 1 : 0);

    // Register callbacks
    win_sparkle_set_can_shutdown_callback([]() -> int {
        // Return 1 if app can shutdown now, 0 otherwise
        // For a video player, check if playback is active
        return 1;  // Allow shutdown for now
    });

    win_sparkle_set_shutdown_request_callback([]() {
        // App should save state and exit
        qInfo() << "WinSparkle requested application shutdown for update";
        QCoreApplication::quit();
    });

    // Initialize WinSparkle
    win_sparkle_init();

    m_initialized = true;
    qInfo() << "WinSparkle initialized successfully";

#else
    qWarning() << "Auto-update not supported on this platform";
#endif
}

void AutoUpdate::checkForUpdates()
{
#ifdef Q_OS_WIN
    if (!m_initialized) {
        qWarning() << "AutoUpdate not initialized, cannot check for updates";
        return;
    }

    qInfo() << "Manually checking for updates...";
    emit updateCheckStarted();

    win_sparkle_check_update_with_ui();
#else
    qWarning() << "Auto-update not supported on this platform";
#endif
}

void AutoUpdate::checkForUpdatesBackground()
{
#ifdef Q_OS_WIN
    if (!m_initialized) {
        qWarning() << "AutoUpdate not initialized, cannot check for updates";
        return;
    }

    qInfo() << "Background update check...";
    emit updateCheckStarted();

    win_sparkle_check_update_without_ui();
#else
    qWarning() << "Auto-update not supported on this platform";
#endif
}

void AutoUpdate::setCheckInterval(int seconds)
{
#ifdef Q_OS_WIN
    if (!m_initialized) {
        qWarning() << "AutoUpdate not initialized";
        return;
    }

    qInfo() << "Setting update check interval to" << seconds << "seconds";
    win_sparkle_set_update_check_interval(seconds);
#else
    Q_UNUSED(seconds);
#endif
}

void AutoUpdate::setAutomaticChecks(bool enabled)
{
#ifdef Q_OS_WIN
    if (!m_initialized) {
        qWarning() << "AutoUpdate not initialized";
        return;
    }

    qInfo() << "Setting automatic update checks:" << enabled;
    win_sparkle_set_automatic_check_for_updates(enabled ? 1 : 0);
#else
    Q_UNUSED(enabled);
#endif
}

QString AutoUpdate::currentVersion() const
{
    return QCoreApplication::applicationVersion();
}

void AutoUpdate::shutdown()
{
#ifdef Q_OS_WIN
    if (m_initialized) {
        qInfo() << "Shutting down WinSparkle";
        win_sparkle_cleanup();
        m_initialized = false;
    }
#endif
}

bool AutoUpdate::isSupported()
{
#ifdef Q_OS_WIN
    return true;
#else
    return false;
#endif
}

// Callback implementations
void AutoUpdate::onUpdateCheckStarted()
{
    if (s_instance) {
        qInfo() << "Update check started";
        emit s_instance->updateCheckStarted();
    }
}

void AutoUpdate::onUpdateCheckFinished()
{
    if (s_instance) {
        qInfo() << "Update check finished";
    }
}

void AutoUpdate::onUpdateFound()
{
    if (s_instance) {
        qInfo() << "Update found";
        emit s_instance->updateAvailable("New version available");
    }
}

void AutoUpdate::onUpdateNotFound()
{
    if (s_instance) {
        qInfo() << "No update found";
        emit s_instance->noUpdateAvailable();
    }
}

void AutoUpdate::onError()
{
    if (s_instance) {
        qWarning() << "Update check error";
        emit s_instance->updateCheckFailed("Failed to check for updates");
    }
}
