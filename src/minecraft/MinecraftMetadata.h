#pragma once

#include <QJsonObject>
#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>

#include "download/DownloadManager.h"
#include "instances/Instance.h"
#include "launcher/LaunchPlan.h"
#include "minecraft/MinecraftVersion.h"

namespace xylar {

class MinecraftMetadata final : public QObject
{
    Q_OBJECT

public:
    explicit MinecraftMetadata(QObject *parent = nullptr);

    [[nodiscard]] QList<MinecraftVersion> refreshVersions(DownloadManager &downloads, QString *errorMessage = nullptr);
    [[nodiscard]] MinecraftVersion createLoaderVersion(const MinecraftVersion &minecraftVersion, const QString &loaderName, DownloadManager &downloads, QString *errorMessage = nullptr);
    [[nodiscard]] MinecraftInstallResult installVersion(const MinecraftVersion &version, const Instance &instance, DownloadManager &downloads);
    [[nodiscard]] LaunchPlan buildLaunchPlan(const Instance &instance, const QString &playerName, int minMemoryMb, int maxMemoryMb, const QString &javaPath, QString *errorMessage = nullptr) const;

signals:
    void logLine(const QString &line);

private:
    [[nodiscard]] QString manifestPath() const;
    [[nodiscard]] QString versionDir(const QString &versionId) const;
    [[nodiscard]] QString versionJsonPath(const QString &versionId) const;
    [[nodiscard]] QString clientJarPath(const QString &versionId) const;
    [[nodiscard]] QString assetIndexPath(const QString &assetIndexId) const;
    [[nodiscard]] QJsonObject readObject(const QString &path, QString *errorMessage = nullptr) const;
    [[nodiscard]] QJsonObject versionJson(const QString &versionId, QString *errorMessage = nullptr) const;
    [[nodiscard]] QJsonObject resolvedVersionJson(const QString &versionId, QString *errorMessage = nullptr) const;
    [[nodiscard]] QJsonObject mergeInheritedVersion(const QJsonObject &parent, const QJsonObject &child) const;
    [[nodiscard]] QString clientVersionId(const QJsonObject &root) const;
    [[nodiscard]] QString offlineUuid(const QString &playerName) const;
    [[nodiscard]] bool rulesAllow(const QJsonArray &rules) const;
    [[nodiscard]] bool ruleMatches(const QJsonObject &rule) const;
    [[nodiscard]] QStringList collectArguments(const QJsonValue &value, const QMap<QString, QString> &variables) const;
    [[nodiscard]] QString replaceVariables(QString value, const QMap<QString, QString> &variables) const;
    [[nodiscard]] QString mavenPath(const QString &name) const;
    [[nodiscard]] QString javaExecutable(const QString &configuredJavaPath) const;

    void collectLibraryDownloads(const QJsonObject &root, QList<DownloadRequest> &requests, QStringList *classpath, QStringList *nativeJars) const;
    [[nodiscard]] bool collectAssetDownloads(const QJsonObject &root, QList<DownloadRequest> &requests, DownloadManager &downloads, QString *errorMessage);
    bool extractNatives(const QStringList &nativeJars, const QString &nativeDir, QString *errorMessage) const;
};

} // namespace xylar
