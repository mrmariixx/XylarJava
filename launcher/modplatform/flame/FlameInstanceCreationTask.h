#pragma once

#include "InstanceCreationTask.h"

#include <optional>

#include "minecraft/MinecraftInstance.h"

#include "modplatform/flame/FileResolvingTask.h"

#include "net/NetJob.h"

#include "ui/dialogs/BlockedModsDialog.h"

class FlameCreationTask final : public InstanceCreationTask {
    Q_OBJECT

   public:
    FlameCreationTask(const QString& staging_path,
                      SettingsObject* global_settings,
                      QWidget* parent,
                      QString id,
                      QString version_id,
                      QString original_instance_id = {})
        : InstanceCreationTask(), m_parent(parent), m_managedId(std::move(id)), m_managedVersionId(std::move(version_id))
    {
        setStagingPath(staging_path);
        setParentSettings(global_settings);

        m_original_instance_id = std::move(original_instance_id);
    }

    bool abort() override;

    bool updateInstance() override;
    std::unique_ptr<MinecraftInstance> createInstance() override;

   private slots:
    void idResolverSucceeded(QEventLoop&);
    void setupDownloadJob(QEventLoop&);
    void copyBlockedMods(QList<BlockedMod> const& blocked_mods);
    void validateOtherResources(QEventLoop& loop);
    QString getVersionForLoader(QString uid, QString loaderType, QString version, QString mcVersion);

   private:
    QWidget* m_parent = nullptr;

    shared_qobject_ptr<Flame::FileResolvingTask> m_modIdResolver;
    Flame::Manifest m_pack;

    // Handle to allow aborting
    Task::Ptr m_processUpdateFileInfoJob = nullptr;
    NetJob::Ptr m_filesJob = nullptr;

    QString m_managedId, m_managedVersionId;

    QList<std::pair<QString, QString>> m_otherResources;

    std::optional<BaseInstance*> m_instance;

    QStringList m_selectedOptionalMods;
};
