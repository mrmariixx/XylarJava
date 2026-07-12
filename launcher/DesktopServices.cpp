#include "DesktopServices.h"
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QProcess>
#include "FileSystem.h"

namespace DesktopServices {
bool openPath(const QFileInfo& path, bool ensureFolderPathExists)
{
    qDebug() << "Opening path" << path;
    if (ensureFolderPathExists) {
        FS::ensureFolderPathExists(path);
    }
    return openUrl(QUrl::fromLocalFile(QFileInfo(path).absoluteFilePath()));
}

bool openPath(const QString& path, bool ensureFolderPathExists)
{
    return openPath(QFileInfo(path), ensureFolderPathExists);
}

bool run(const QString& application, const QStringList& args, const QString& workingDirectory, qint64* pid)
{
    qDebug() << "Running" << application << "with args" << args.join(' ');
    return QProcess::startDetached(application, args, workingDirectory, pid);
}

bool openUrl(const QUrl& url)
{
    qDebug() << "Opening URL" << url.toString();
    return QDesktopServices::openUrl(url);
}

bool isFlatpak()
{
#ifdef Q_OS_LINUX
    return QFile::exists("/.flatpak-info");
#else
    return false;
#endif
}

bool isSnap()
{
#ifdef Q_OS_LINUX
    return getenv("SNAP");
#else
    return false;
#endif
}

}  // namespace DesktopServices
