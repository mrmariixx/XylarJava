#include "launcher/LauncherController.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QProcess>
#include <QUrl>

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
    return installInstance(versionId, QStringLiteral("Vanilla"), instanceName);
}

bool LauncherController::installInstance(const QString &versionId, const QString &loaderName, const QString &instanceName)
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
    const QString resolvedName = instanceName.trimmed().isEmpty()
        ? QStringLiteral("%1 %2").arg(loaderName, version.id)
        : instanceName.trimmed();
    Instance instance = m_instanceStore.ensureInstance(version.id, resolvedName, &error);
    if (!instance.isValid()) {
        emit logLine(error);
        return false;
    }
    instance.minecraftVersion = version;
    instance.loader = loaderName;
    instance.lastLaunch = QDateTime::currentMSecsSinceEpoch();
    if (!m_instanceStore.saveInstance(instance, &error)) {
        emit logLine(error);
        return false;
    }

    emit logLine(QStringLiteral("Installing Minecraft %1...").arg(version.id));
    MinecraftInstallResult result = m_metadata.installVersion(version, instance, m_downloadManager);
    emit logLine(result.message);
    if (!result.ok) {
        emit instancesChanged(instances());
        return false;
    }

    if (loaderName.compare(QStringLiteral("Vanilla"), Qt::CaseInsensitive) != 0) {
        QString installerJava;
        if (loaderName.compare(QStringLiteral("Forge"), Qt::CaseInsensitive) == 0) {
            installerJava = ensureJavaRuntime();
            if (installerJava.isEmpty()) {
                emit instancesChanged(instances());
                return false;
            }
        }
        MinecraftVersion loaderVersion = m_metadata.createLoaderVersion(version, loaderName, m_downloadManager, installerJava, &error);
        if (loaderVersion.id.isEmpty()) {
            emit logLine(error);
            emit instancesChanged(instances());
            return false;
        }
        instance.minecraftVersion = loaderVersion;
        instance.loader = loaderName;
        if (!m_instanceStore.saveInstance(instance, &error)) {
            emit logLine(error);
            emit instancesChanged(instances());
            return false;
        }
        if (loaderName.compare(QStringLiteral("Forge"), Qt::CaseInsensitive) == 0) {
            emit logLine(QStringLiteral("Installed Forge profile %1.").arg(loaderVersion.id));
            result.ok = true;
            result.message = QStringLiteral("Installed Forge for %1.").arg(version.id);
        } else {
            emit logLine(QStringLiteral("Installing %1 loader for %2...").arg(loaderName, version.id));
            result = m_metadata.installVersion(loaderVersion, instance, m_downloadManager);
            emit logLine(result.message);
        }
    }

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
        java = ensureJavaRuntime();
        if (java.isEmpty()) {
            return false;
        }
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

bool LauncherController::importModrinthPack(const QString &packPath)
{
    QString error;
    const ModpackImportPlan plan = m_modpackManager.readMrpack(packPath, &error);
    if (plan.minecraftVersion.isEmpty()) {
        emit logLine(error.isEmpty() ? QStringLiteral("Invalid .mrpack file.") : error);
        return false;
    }

    emit logLine(QStringLiteral("Importing %1 (%2, %3).").arg(plan.name, plan.minecraftVersion, plan.loaderName));
    if (!installInstance(plan.minecraftVersion, plan.loaderName, plan.name)) {
        return false;
    }

    Instance instance;
    for (const Instance &candidate : instances()) {
        if (candidate.name == plan.name) {
            instance = candidate;
            break;
        }
    }
    if (!instance.isValid()) {
        emit logLine(QStringLiteral("Installed instance was not found after import."));
        return false;
    }

    QList<DownloadRequest> requests;
    for (const ModpackFile &file : plan.files) {
        DownloadRequest request;
        request.url = QUrl(file.downloads.first());
        request.targetPath = QDir(instance.gamePath).filePath(file.path);
        request.sha1 = file.sha1;
        request.sha512 = file.sha512;
        request.expectedSize = file.size;
        request.label = file.path;
        requests.append(request);
    }

    m_downloadManager.beginBatch(requests.size());
    for (const DownloadRequest &request : requests) {
        if (!m_downloadManager.downloadToFile(request, &error)) {
            emit logLine(error);
            return false;
        }
        QCoreApplication::processEvents();
    }

    if (!m_modpackManager.copyOverrides(plan, instance, &error)) {
        emit logLine(error);
        return false;
    }

    emit logLine(QStringLiteral("Imported %1 with %2 file(s).").arg(plan.name).arg(plan.files.size()));
    emit instancesChanged(instances());
    return true;
}

LaunchPlan LauncherController::lastLaunchPlan() const
{
    return m_lastLaunchPlan;
}

QString LauncherController::ensureJavaRuntime()
{
    QString java = m_javaRuntimeManager.bundledOracleJavaExecutable();
    if (!java.isEmpty()) {
        return java;
    }

    emit logLine(QStringLiteral("Downloading Oracle JDK 21 runtime..."));
    QString error;
    java = m_javaRuntimeManager.ensureOracleJdk21(m_downloadManager, &error);
    if (!java.isEmpty()) {
        emit logLine(QStringLiteral("Oracle JDK 21 ready: %1").arg(java));
        return java;
    }

    emit logLine(error.isEmpty() ? QStringLiteral("Oracle JDK 21 could not be installed.") : error);
    java = m_javaRuntimeManager.preferredJavaExecutable();
    if (!java.isEmpty()) {
        emit logLine(QStringLiteral("Falling back to system Java: %1").arg(java));
        return java;
    }
    return {};
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
