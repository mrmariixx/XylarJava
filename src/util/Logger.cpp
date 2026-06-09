#include "util/Logger.h"

#include <QDateTime>
#include <QDebug>

namespace xylar {
namespace {

void messageHandler(QtMsgType type, const QMessageLogContext &, const QString &message)
{
    const QString level = [type]() {
        switch (type) {
        case QtDebugMsg:
            return QStringLiteral("debug");
        case QtInfoMsg:
            return QStringLiteral("info");
        case QtWarningMsg:
            return QStringLiteral("warning");
        case QtCriticalMsg:
            return QStringLiteral("critical");
        case QtFatalMsg:
            return QStringLiteral("fatal");
        }
        return QStringLiteral("log");
    }();

    const QString line = QStringLiteral("[%1] [%2] %3")
        .arg(QDateTime::currentDateTime().toString(Qt::ISODateWithMs), level, message);
    fprintf(stderr, "%s\n", qPrintable(line));
    if (type == QtFatalMsg) {
        abort();
    }
}

} // namespace

void Logger::installMessageHandler()
{
    qInstallMessageHandler(messageHandler);
}

void Logger::info(const QString &message)
{
    qInfo().noquote() << message;
}

void Logger::warning(const QString &message)
{
    qWarning().noquote() << message;
}

} // namespace xylar
