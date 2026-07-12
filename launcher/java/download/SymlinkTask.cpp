#include "java/download/SymlinkTask.h"
#include <QFileInfo>

#include "FileSystem.h"

namespace Java {
SymlinkTask::SymlinkTask(QString final_path) : m_path(final_path) {}

QString findBinPath(QString root, QString pattern)
{
    auto path = FS::PathCombine(root, pattern);
    if (QFileInfo::exists(path)) {
        return path;
    }

    auto entries = QDir(root).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (auto& entry : entries) {
        path = FS::PathCombine(entry.absoluteFilePath(), pattern);
        if (QFileInfo::exists(path)) {
            return path;
        }
    }

    return {};
}

void SymlinkTask::executeTask()
{
    setStatus(tr("Checking for Java binary path"));
    const auto binPath = FS::PathCombine("bin", "java");
    const auto wantedPath = FS::PathCombine(m_path, binPath);
    if (QFileInfo::exists(wantedPath)) {
        emitSucceeded();
        return;
    }

    setStatus(tr("Searching for Java binary path"));
    const auto contentsPartialPath = FS::PathCombine("Contents", "Home", binPath);
    const auto relativePathToBin = findBinPath(m_path, contentsPartialPath);
    if (relativePathToBin.isEmpty()) {
        emitFailed(tr("Failed to find Java binary path"));
        return;
    }
    const auto folderToLink = relativePathToBin.chopped(binPath.length());

    setStatus(tr("Collecting folders to symlink"));
    auto entries = QDir(folderToLink).entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);
    QList<FS::LinkPair> files;
    setProgress(0, entries.length());
    for (auto& entry : entries) {
        files.append({ entry.absoluteFilePath(), FS::PathCombine(m_path, entry.fileName()) });
    }

    setStatus(tr("Symlinking Java binary path"));
    FS::create_link folderLink(files);
    connect(&folderLink, &FS::create_link::fileLinked, [this](QString src, QString dst) { setProgress(m_progress + 1, m_progressTotal); });
    if (!folderLink()) {
        emitFailed(folderLink.getOSError().message().c_str());
    } else {
        emitSucceeded();
    }
}

}  // namespace Java
