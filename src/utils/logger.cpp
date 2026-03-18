#include "logger.h"

#include <QDateTime>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

Logger &Logger::instance()
{
    static Logger logger;
    return logger;
}

void Logger::init()
{
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(logDir);

    QString logPath = logDir + "/iina.log";
    m_logFile.setFileName(logPath);
    m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
}

void Logger::write(const QString &message)
{
    QMutexLocker lock(&m_mutex);
    if (m_logFile.isOpen()) {
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        m_logFile.write(QString("[%1] %2\n").arg(timestamp, message).toUtf8());
        m_logFile.flush();
    }
}

void Logger::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context)
    QString level;
    switch (type) {
    case QtDebugMsg:    level = "DEBUG"; break;
    case QtInfoMsg:     level = "INFO"; break;
    case QtWarningMsg:  level = "WARN"; break;
    case QtCriticalMsg: level = "ERROR"; break;
    case QtFatalMsg:    level = "FATAL"; break;
    }

    QString formatted = QString("[%1] %2").arg(level, msg);
    Logger::instance().write(formatted);

#ifdef QT_DEBUG
    fprintf(stderr, "%s\n", qPrintable(formatted));
#endif
}
