#pragma once

#include <QFuture>
#include <QFutureWatcher>
#include "BaseInstance.h"
#include "MMCZip.h"
#include "minecraft/MinecraftInstance.h"
#include "modplatform/ModIndex.h"
#include "modplatform/modrinth/ModrinthAPI.h"
#include "tasks/Task.h"

class ModrinthPackExportTask : public Task {
    Q_OBJECT
   public:
    ModrinthPackExportTask(const QString& name,
                           const QString& version,
                           const QString& summary,
                           bool optionalFiles,
                           BaseInstance* instance,
                           const QString& output,
                           MMCZip::FilterFileFunction filter);

   protected:
    void executeTask() override;
    bool abort() override;

   private:
    struct ResolvedFile {
        QString sha1, sha512, url;
        qint64 size;
        ModPlatform::Side side;
    };

    static const QStringList PREFIXES;
    static const QStringList FILE_EXTENSIONS;

    // inputs
    const QString name, version, summary;
    const bool optionalFiles;
    const BaseInstance* instance;
    MinecraftInstance* mcInstance;
    const QDir gameRoot;
    const QString output;
    const MMCZip::FilterFileFunction filter;

    ModrinthAPI api;
    QFileInfoList files;
    QMap<QString, QString> pendingHashes;
    QMap<QString, ResolvedFile> resolvedFiles;
    Task::Ptr task;

    void collectFiles();
    void collectHashes();
    void makeApiRequest();
    void parseApiResponse(QByteArray* response);
    void buildZip();

    QByteArray generateIndex();
};
