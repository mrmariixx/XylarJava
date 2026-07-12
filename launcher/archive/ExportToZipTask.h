#pragma once

#include <QDir>
#include <QFileInfoList>
#include <QFuture>
#include <QFutureWatcher>

#include "archive/ArchiveWriter.h"
#include "tasks/Task.h"

namespace MMCZip {
class ExportToZipTask : public Task {
    Q_OBJECT
   public:
    ExportToZipTask(QString outputPath, QDir dir, QFileInfoList files, QString destinationPrefix = "", bool followSymlinks = false)
        : m_outputPath(outputPath)
        , m_output(outputPath)
        , m_dir(dir)
        , m_files(files)
        , m_destinationPrefix(destinationPrefix)
        , m_followSymlinks(followSymlinks)
    {
        setAbortable(true);
    };
    ExportToZipTask(QString outputPath, QString dir, QFileInfoList files, QString destinationPrefix = "", bool followSymlinks = false)
        : ExportToZipTask(outputPath, QDir(dir), files, destinationPrefix, followSymlinks) {};

    virtual ~ExportToZipTask() = default;

    void setExcludeFiles(QStringList excludeFiles) { m_excludeFiles = excludeFiles; }
    void addExtraFile(QString fileName, QByteArray data) { m_extraFiles.insert(fileName, data); }

    using ZipResult = std::optional<QString>;

   protected:
    virtual void executeTask() override;
    bool abort() override;

    ZipResult exportZip();
    void finish();

   private:
    QString m_outputPath;
    ArchiveWriter m_output;
    QDir m_dir;
    QFileInfoList m_files;
    QString m_destinationPrefix;
    bool m_followSymlinks;
    QStringList m_excludeFiles;
    QHash<QString, QByteArray> m_extraFiles;

    QFuture<ZipResult> m_buildZipFuture;
    QFutureWatcher<ZipResult> m_buildZipWatcher;
};
}  // namespace MMCZip
