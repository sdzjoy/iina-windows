#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QFile>
#include <QMutex>

class Logger
{
public:
    static Logger &instance();
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    void init();
    void write(const QString &message);

private:
    Logger() = default;
    QFile m_logFile;
    QMutex m_mutex;
};

#endif // LOGGER_H
