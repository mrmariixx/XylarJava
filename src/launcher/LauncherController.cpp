#include "launcher/LauncherController.h"

#include <QDateTime>
#include <QProcess>

namespace xylar {

LauncherController::LauncherController(QObject *parent)
    : QObject(parent)
    , m_latestVersion(MinecraftVersion::latestReleasePlaceholder())
{
    connect(&m_downloadManager, &DownloadManager::logLine, this, &LauncherController::logLine);
    connect(&m_downloadManager, &DownloadManager::progressChanged, this, &LauncherController::progressChanged);
    connect(&m_metadata, &MinecraftMetadata::logLine, this, &LauncherController::logLine);
}

QString LauncherController::instanceSummary() const
{
    const QList<Instance> loaded = instances();
    if (loaded.isEmpty()) {
        return QStringLiteral("No instances yet. Pick a version and install it.");
    }
    return QStringLiteral("%1 instance(s), Java: %2.").arg(loaded.size()).arg(m_javaRuntimeManager.preferredRuntimeName());
}

QString LauncherController::accountSummary() const
{
    return m_authManager.currentAccountLabel();
}

Instance LauncherController::defaultInstance() const
{
    Instance instance;
    instance.id = QStringLiteral("vanilla-latest");
    instance.name = QStringLiteral("Vanilla latest");
    instance.minecraftVersion = m_latestVersion;
    instance.loader = QStringLiteral("Vanilla");
    return instance;
}

QList<MinecraftVersion> LauncherController::cachedVersions() const
{
    return m_versions;
}

QList<Instance> LauncherController::instances() const
{
    return m_instanceStore.loadInstances();
}

QString LauncherController::javaPath() const
{
    return m_javaRuntimeManager.preferredJavaExecutable();
}

bool LauncherController::refreshVersions()
{
    QString error;
    emit logLine(QStringLiteral("Refreshing Minecraft versions..."));
    m_versions = m_metadata.refreshVersions(m_downloadManager, &error);
    if (m_versions.isEmpty()) {
        emit logLine(error.isEmpty() ? QStringLiteral("No versions loaded.") : error);
        return false;
    }

    emit versionsChanged(m_versions);
    return true;
}

bool LauncherController::installVanilla(const QString &versionId, const QString &instanceName)
{
    MinecraftVersion version = versionById(versionId);
    if (version.id.isEmpty()) {
        if (!refreshVersions()) {
            return false;
        }
        version = versionById(versionId);
    }
    if (version.id.isEmpty()) {
        emit logLine(QStringLiteral("Version not found: %1").arg(versionId));
        return false;
    }

    QString error;
    Instance instance = m_instanceStore.ensureInstance(version.id, instanceName, &error);
    if (!instance.isValid()) {
        emit logLine(error);
        return false;
    }
    instance.minecraftVersion = version;
    instance.lastLaunch = QDateTime::currentMSecsSinceEpoch();
    if (!m_instanceStore.saveInstance(instance, &error)) {
        emit logLine(error);
        return false;
    }

    emit logLine(QStringLiteral("Installing %1...").arg(version.id));
    const MinecraftInstallResult result = m_metadata.installVersion(version, instance, m_downloadManager);
    emit logLine(result.message);
    emit instancesChanged(instances());
    return result.ok;
}

bool LauncherController::launchInstance(const QString &instanceId, const QString &playerName, int minMemoryMb, int maxMemoryMb, const QString &customJavaPath)
{
    const Instance instance = m_instanceStore.loadInstance(instanceId);
    if (!instance.isValid()) {
        emit logLine(QStringLiteral("Instance not found: %1").arg(instanceId));
        return false;
    }

    QString java = customJavaPath.trimmed();
    if (java.isEmpty()) {
        java = m_javaRuntimeManager.preferredJavaExecutable();
    }

    QString error;
    m_lastLaunchPlan = m_metadata.buildLaunchPlan(instance, playerName, minMemoryMb, maxMemoryMb, java, &error);
    if (!m_lastLaunchPlan.isValid()) {
        emit logLine(error.isEmpty() ? QStringLiteral("Could not build launch command.") : error);
        return false;
    }

    emit logLine(QStringLiteral("Launching %1 with %2").arg(instance.name, m_lastLaunchPlan.versionId));
    emit logLine(m_lastLaunchPlan.displayCommand());
    const bool started = QProcess::startDetached(m_lastLaunchPlan.program, m_lastLaunchPlan.arguments, m_lastLaunchPlan.workingDirectory);
    if (!started) {
        emit logLine(QStringLiteral("Java process did not start. Check Java path in Settings."));
        return false;
    }
    return true;
}

LaunchPlan LauncherController::lastLaunchPlan() const
{
    return m_lastLaunchPlan;
}

MinecraftVersion LauncherController::versionById(const QString &versionId) const
{
    for (const MinecraftVersion &version : m_versions) {
        if (version.id == versionId) {
            return version;
        }
    }
    return {};
}

} // namespace xylar
