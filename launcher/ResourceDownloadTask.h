#pragma once

#include "net/NetJob.h"
#include "tasks/SequentialTask.h"

#include "minecraft/mod/tasks/LocalResourceUpdateTask.h"
#include "modplatform/ModIndex.h"

class ResourceFolderModel;

class ResourceDownloadTask : public SequentialTask {
    Q_OBJECT
   public:
    explicit ResourceDownloadTask(ModPlatform::IndexedPack::Ptr pack,
                                  ModPlatform::IndexedVersion version,
                                  ResourceFolderModel* packs,
                                  bool is_indexed = true);
    const QString& getFilename() const { return m_pack_version.fileName; }
    const QVariant& getVersionID() const { return m_pack_version.fileId; }
    const ModPlatform::IndexedVersion& getVersion() const { return m_pack_version; }
    const ModPlatform::ResourceProvider& getProvider() const { return m_pack->provider; }
    const QString& getName() const { return m_pack->name; }
    ModPlatform::IndexedPack::Ptr getPack() { return m_pack; }

   private:
    ModPlatform::IndexedPack::Ptr m_pack;
    ModPlatform::IndexedVersion m_pack_version;
    ResourceFolderModel* m_pack_model;

    NetJob::Ptr m_filesNetJob;
    LocalResourceUpdateTask::Ptr m_update_task;

    void downloadProgressChanged(qint64 current, qint64 total);
    void downloadFailed(QString reason);
    void downloadSucceeded();

    std::tuple<QString, QString> to_delete{ "", "" };

   private slots:
    void hasOldResource(QString name, QString filename);
};
