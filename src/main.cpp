#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QIcon>
#include <QSurfaceFormat>
#include <QTranslator>
#include <QLocale>
#include <QDir>

#include "application.h"
#include "utils/logger.h"

int main(int argc, char *argv[])
{
    // Set OpenGL as the default rendering backend for mpv compatibility
    QSurfaceFormat format;
    format.setMajorVersion(3);
    format.setMinorVersion(3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    format.setDepthBufferSize(24);
    QSurfaceFormat::setDefaultFormat(format);

    QApplication app(argc, argv);
    app.setApplicationName("IINA");
    app.setApplicationVersion("0.1.0");
    app.setOrganizationName("IINA");
    app.setOrganizationDomain("iina.io");
    app.setWindowIcon(QIcon(":/resources/icons/iina.svg"));

    // Initialize logging
    Logger::instance().init();
    qInstallMessageHandler(Logger::messageHandler);

    qInfo() << "IINA for Windows" << app.applicationVersion() << "starting";

    // ─── Load translations ───
    QTranslator translator;
    QLocale locale = QLocale::system();

    // Try to load translation from embedded resources first, then from filesystem
    // The .qm file is compiled from translations/iina_zh_CN.ts
    QString tsName = "iina_" + locale.name();  // e.g. "iina_zh_CN"
    if (translator.load(tsName, ":/translations")) {
        app.installTranslator(&translator);
        qInfo() << "Loaded translation:" << tsName;
    } else if (translator.load(tsName, QApplication::applicationDirPath() + "/translations")) {
        app.installTranslator(&translator);
        qInfo() << "Loaded translation from filesystem:" << tsName;
    } else {
        qInfo() << "No translation found for locale:" << locale.name() << "- using English";
    }

    // Set Material dark theme
    QQuickStyle::setStyle("Material");

    // Initialize application
    Application iina;
    QQmlApplicationEngine engine;

    iina.init(engine);

    // Handle command line arguments (open files passed as args)
    QMetaObject::invokeMethod(&iina, "processCommandLine", Qt::QueuedConnection);

    return app.exec();
}
