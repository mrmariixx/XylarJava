#pragma once

#include "FTBPackManifest.h"

#include "InstanceTask.h"
#include "QObjectPtr.h"
#include "modplatform/flame/FileResolvingTask.h"
#include "net/NetJob.h"
#include "ui/dialogs/BlockedModsDialog.h"

#include <QWidget>

namespace FTB {

class PackInstallTask final : public InstanceTask {
    Q_OBJECT

   public:
    explicit PackInstallTask(Modpack pack, QString version, QWidget* parent = nullptr);
    ~PackInstallTask() override = default;

    bool abort() override;

   protected:
    void executeTask() override;

   private slots:
    void onManifestDownloadSucceeded(QByteArray* responsePtr);
    void onResolveModsSucceeded();
    void onCreateInstanceSucceeded();
    void onModDownloadSucceeded();

    void onManifestDownloadFailed(QString reason);
    void onResolveModsFailed(QString reason);
    void onCreateInstanceFailed(QString reason);
    void onModDownloadFailed(QString reason);

   private:
    void resolveMods();
    void createInstance();
    void downloadPack();
    void copyBlockedMods();

   private:
    NetJob::Ptr m_net_job = nullptr;
    shared_qobject_ptr<Flame::FileResolvingTask> m_modIdResolverTask = nullptr;

    QList<int> m_fileIds;

    Modpack m_pack;
    QString m_versionName;
    Version m_version;

    QMap<QString, QString> m_filesToCopy;
    QList<BlockedMod> m_blockedMods;

    // FIXME: nuke
    QWidget* m_parent;
};

}  // namespace FTB
