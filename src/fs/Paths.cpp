#include "fs/Paths.h"

#include <QCoreApplication>
#include <QStandardPaths>

namespace xylar {
namespace {

QDir ensureDir(const QString &path)
{
    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath(QStringLiteral("."));
    }
    return dir;
}

} // namespace

QDir Paths::appDataDir()
{
    const QString appDir = QCoreApplication::applicationDirPath();
    return ensureDir(QDir(appDir).filePath(QStringLiteral("data")));
}

QDir Paths::metadataDir()
{
    return ensureDir(appDataDir().filePath(QStringLiteral("meta")));
}

QDir Paths::versionsDir()
{
    return ensureDir(appDataDir().filePath(QStringLiteral("versions")));
}

QDir Paths::instancesDir()
{
    return ensureDir(appDataDir().filePath(QStringLiteral("instances")));
}

QDir Paths::librariesDir()
{
    return ensureDir(appDataDir().filePath(QStringLiteral("libraries")));
}

QDir Paths::assetsDir()
{
    return ensureDir(appDataDir().filePath(QStringLiteral("assets")));
}

QDir Paths::runtimesDir()
{
    return ensureDir(appDataDir().filePath(QStringLiteral("runtimes")));
}

} // namespace xylar
