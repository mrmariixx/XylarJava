#pragma once

#include <QList>
#include <QObject>
#include <QString>

#include "auth/AuthManager.h"
#include "download/DownloadManager.h"
#include "instances/Instance.h"
#include "instances/InstanceStore.h"
#include "java/JavaRuntimeManager.h"
#include "launcher/LaunchPlan.h"
#include "minecraft/MinecraftMetadata.h"
#include "minecraft/MinecraftVersion.h"
#include "modpacks/ModpackManager.h"

namespace xylar {

class LauncherController final : public QObject
{
    Q_OBJECT

public:
    explicit LauncherController(QObject *parent = nullptr);

    [[nodiscard]] QString instanceSummary() const;
    [[nodiscard]] QString accountSummary() const;
    [[nodiscard]] Instance defaultInstance() const;
    [[nodiscard]] QList<MinecraftVersion> cachedVersions() const;
    [[nodiscard]] QList<Instance> instances() const;
    [[nodiscard]] QString javaPath() const;

    bool refreshVersions();
    bool installInstance(const QString &versionId, const QString &loaderName, const QString &instanceName);
    bool installVanilla(const QString &versionId, const QString &instanceName);
    bool importModrinthPack(const QString &packPath);
    bool launchInstance(const QString &instanceId, const QString &playerName, int minMemoryMb, int maxMemoryMb, const QString &customJavaPath);
    [[nodiscard]] LaunchPlan lastLaunchPlan() const;
    [[nodiscard]] QString ensureJavaRuntime();

signals:
    void logLine(const QString &line);
    void progressChanged(int value, int maximum, const QString &label);
    void versionsChanged(const QList<MinecraftVersion> &versions);
    void instancesChanged(const QList<Instance> &instances);

private:
    [[nodiscard]] MinecraftVersion versionById(const QString &versionId) const;

    AuthManager m_authManager;
    JavaRuntimeManager m_javaRuntimeManager;
    DownloadManager m_downloadManager;
    InstanceStore m_instanceStore;
    MinecraftMetadata m_metadata;
    ModpackManager m_modpackManager;
    QList<MinecraftVersion> m_versions;
    MinecraftVersion m_latestVersion;
    LaunchPlan m_lastLaunchPlan;
};

} // namespace xylar
