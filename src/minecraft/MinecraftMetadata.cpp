#include "minecraft/MinecraftMetadata.h"

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QProcess>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QSysInfo>
#include <QtGlobal>
#include <QUuid>

#include "fs/Paths.h"

namespace xylar {
namespace {

const QUrl kManifestUrl(QStringLiteral("https://piston-meta.mojang.com/mc/game/version_manifest_v2.json"));
const QString kFabricMeta = QStringLiteral("https://meta.fabricmc.net/v2/versions/loader/%1");
const QString kFabricProfile = QStringLiteral("https://meta.fabricmc.net/v2/versions/loader/%1/%2/profile/json");
const QString kQuiltMeta = QStringLiteral("https://meta.quiltmc.org/v3/versions/loader/%1");
const QString kQuiltProfile = QStringLiteral("https://meta.quiltmc.org/v3/versions/loader/%1/%2/profile/json");

QString osName()
{
#if defined(Q_OS_WIN)
    return QStringLiteral("windows");
#elif defined(Q_OS_MACOS)
    return QStringLiteral("osx");
#else
    return QStringLiteral("linux");
#endif
}

DownloadRequest requestFromJson(const QJsonObject &download, const QString &targetPath, const QString &label)
{
    DownloadRequest request;
    request.url = QUrl(download.value(QStringLiteral("url")).toString());
    request.targetPath = targetPath;
    request.sha1 = download.value(QStringLiteral("sha1")).toString();
    request.expectedSize = static_cast<qint64>(download.value(QStringLiteral("size")).toDouble(-1));
    request.label = label;
    return request;
}

QString classpathSeparator()
{
#if defined(Q_OS_WIN)
    return QStringLiteral(";");
#else
    return QStringLiteral(":");
#endif
}

QString quoteForPowerShell(QString value)
{
    value.replace(QStringLiteral("'"), QStringLiteral("''"));
    return QStringLiteral("'") + value + QStringLiteral("'");
}

} // namespace

MinecraftMetadata::MinecraftMetadata(QObject *parent)
    : QObject(parent)
{
}

QList<MinecraftVersion> MinecraftMetadata::refreshVersions(DownloadManager &downloads, QString *errorMessage)
{
    DownloadRequest request;
    request.url = kManifestUrl;
    request.targetPath = manifestPath();
    request.label = QStringLiteral("Minecraft version manifest");
    request.force = true;

    if (!downloads.downloadToFile(request, errorMessage)) {
        return {};
    }

    const QJsonObject root = readObject(manifestPath(), errorMessage);
    const QJsonArray versions = root.value(QStringLiteral("versions")).toArray();
    QList<MinecraftVersion> result;
    result.reserve(versions.size());
    for (const QJsonValue &value : versions) {
        const QJsonObject item = value.toObject();
        MinecraftVersion version;
        version.id = item.value(QStringLiteral("id")).toString();
        version.type = item.value(QStringLiteral("type")).toString();
        version.url = item.value(QStringLiteral("url")).toString();
        version.releaseTime = item.value(QStringLiteral("releaseTime")).toString();
        if (!version.id.isEmpty() && !version.url.isEmpty()) {
            result.append(version);
        }
    }
    emit logLine(QStringLiteral("Loaded %1 Minecraft versions.").arg(result.size()));
    return result;
}

MinecraftVersion MinecraftMetadata::createLoaderVersion(const MinecraftVersion &minecraftVersion, const QString &loaderName, DownloadManager &downloads, QString *errorMessage)
{
    const QString normalized = loaderName.trimmed().toLower();
    if (normalized == QStringLiteral("vanilla") || normalized.isEmpty()) {
        return minecraftVersion;
    }

    const bool fabric = normalized == QStringLiteral("fabric");
    const bool quilt = normalized == QStringLiteral("quilt");
    if (!fabric && !quilt) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("%1 loader install is not wired yet.").arg(loaderName);
        }
        return {};
    }

    const QString listPath = Paths::metadataDir().filePath(QStringLiteral("%1-%2-loaders.json").arg(normalized, minecraftVersion.id));
    DownloadRequest listRequest;
    listRequest.url = QUrl((fabric ? kFabricMeta : kQuiltMeta).arg(minecraftVersion.id));
    listRequest.targetPath = listPath;
    listRequest.label = QStringLiteral("%1 loader list for %2").arg(loaderName, minecraftVersion.id);
    listRequest.force = true;

    QString error;
    downloads.beginBatch(1);
    if (!downloads.downloadToFile(listRequest, &error)) {
        if (errorMessage) {
            *errorMessage = error;
        }
        return {};
    }

    QFile file(listPath);
    if (!file.open(QIODevice::ReadOnly)) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Cannot read loader metadata for %1.").arg(loaderName);
        }
        return {};
    }

    const QJsonDocument document = QJsonDocument::fromJson(file.readAll());
    const QJsonArray loaders = document.array();
    QString loaderVersion;
    for (const QJsonValue &value : loaders) {
        const QJsonObject loader = value.toObject().value(QStringLiteral("loader")).toObject();
        if (loader.value(QStringLiteral("stable")).toBool(true)) {
            loaderVersion = loader.value(QStringLiteral("version")).toString();
            break;
        }
    }
    if (loaderVersion.isEmpty() && !loaders.isEmpty()) {
        loaderVersion = loaders.first().toObject().value(QStringLiteral("loader")).toObject().value(QStringLiteral("version")).toString();
    }
    if (loaderVersion.isEmpty()) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("No %1 loader found for %2.").arg(loaderName, minecraftVersion.id);
        }
        return {};
    }

    MinecraftVersion loaderVersionInfo;
    loaderVersionInfo.id = QStringLiteral("%1-%2-%3").arg(normalized, loaderVersion, minecraftVersion.id);
    loaderVersionInfo.type = QStringLiteral("loader");
    loaderVersionInfo.url = (fabric ? kFabricProfile : kQuiltProfile).arg(minecraftVersion.id, loaderVersion);
    loaderVersionInfo.releaseTime = minecraftVersion.releaseTime;
    return loaderVersionInfo;
}

MinecraftInstallResult MinecraftMetadata::installVersion(const MinecraftVersion &version, const Instance &instance, DownloadManager &downloads)
{
    MinecraftInstallResult result;
    QString error;
    QList<DownloadRequest> requests;

    QDir().mkpath(versionDir(version.id));
    QDir().mkpath(instance.gamePath);

    DownloadRequest versionRequest;
    versionRequest.url = QUrl(version.url);
    versionRequest.targetPath = versionJsonPath(version.id);
    versionRequest.label = QStringLiteral("Version %1 metadata").arg(version.id);
    requests.append(versionRequest);

    downloads.beginBatch(1);
    if (!downloads.downloadToFile(versionRequest, &error)) {
        result.message = error;
        return result;
    }

    const QJsonObject root = resolvedVersionJson(version.id, &error);
    if (root.isEmpty()) {
        result.message = error;
        return result;
    }

    const QJsonObject downloadsObject = root.value(QStringLiteral("downloads")).toObject();
    const QJsonObject clientDownload = downloadsObject.value(QStringLiteral("client")).toObject();
    if (!clientDownload.isEmpty()) {
        const QString clientId = clientVersionId(root);
        requests.append(requestFromJson(clientDownload, clientJarPath(clientId), QStringLiteral("Client jar %1").arg(clientId)));
    }

    QStringList classpath;
    QStringList nativeJars;
    collectLibraryDownloads(root, requests, &classpath, &nativeJars);
    if (!collectAssetDownloads(root, requests, downloads, &error)) {
        result.message = error;
        return result;
    }

    result.plannedDownloads = requests.size();
    downloads.beginBatch(requests.size());
    for (const DownloadRequest &request : requests) {
        if (!downloads.downloadToFile(request, &error)) {
            result.message = error;
            return result;
        }
        QCoreApplication::processEvents();
    }

    const QString nativeDir = QDir(instance.rootPath).filePath(QStringLiteral("natives"));
    if (!extractNatives(nativeJars, nativeDir, &error)) {
        result.message = error;
        return result;
    }

    result.ok = true;
    result.message = QStringLiteral("Installed %1 for %2.").arg(version.id, instance.name);
    return result;
}

LaunchPlan MinecraftMetadata::buildLaunchPlan(const Instance &instance, const QString &playerName, int minMemoryMb, int maxMemoryMb, const QString &javaPath, QString *errorMessage) const
{
    LaunchPlan plan;
    const QJsonObject root = resolvedVersionJson(instance.minecraftVersion.id, errorMessage);
    if (root.isEmpty()) {
        return plan;
    }

    QStringList classpath;
    QStringList nativeJars;
    QList<DownloadRequest> ignored;
    collectLibraryDownloads(root, ignored, &classpath, &nativeJars);
    classpath.append(clientJarPath(clientVersionId(root)));

    const QJsonObject assetIndex = root.value(QStringLiteral("assetIndex")).toObject();
    const QString assetIndexId = assetIndex.value(QStringLiteral("id")).toString(root.value(QStringLiteral("assets")).toString());
    const QString nativeDir = QDir(instance.rootPath).filePath(QStringLiteral("natives"));
    const QString player = playerName.trimmed().isEmpty() ? QStringLiteral("Player") : playerName.trimmed();
    const QString versionType = root.value(QStringLiteral("type")).toString(QStringLiteral("release"));

    QMap<QString, QString> variables;
    variables.insert(QStringLiteral("natives_directory"), QDir::toNativeSeparators(nativeDir));
    variables.insert(QStringLiteral("launcher_name"), QStringLiteral("XylarJava"));
    variables.insert(QStringLiteral("launcher_version"), QStringLiteral(XYLARJAVA_VERSION));
    variables.insert(QStringLiteral("classpath"), QDir::toNativeSeparators(classpath.join(classpathSeparator())));
    variables.insert(QStringLiteral("auth_player_name"), player);
    variables.insert(QStringLiteral("version_name"), instance.minecraftVersion.id);
    variables.insert(QStringLiteral("game_directory"), QDir::toNativeSeparators(instance.gamePath));
    variables.insert(QStringLiteral("assets_root"), QDir::toNativeSeparators(Paths::assetsDir().absolutePath()));
    variables.insert(QStringLiteral("assets_index_name"), assetIndexId);
    variables.insert(QStringLiteral("auth_uuid"), offlineUuid(player));
    variables.insert(QStringLiteral("auth_access_token"), QStringLiteral("0"));
    variables.insert(QStringLiteral("clientid"), QString());
    variables.insert(QStringLiteral("auth_xuid"), QString());
    variables.insert(QStringLiteral("user_type"), QStringLiteral("legacy"));
    variables.insert(QStringLiteral("version_type"), versionType);
    variables.insert(QStringLiteral("resolution_width"), QStringLiteral("854"));
    variables.insert(QStringLiteral("resolution_height"), QStringLiteral("480"));
    variables.insert(QStringLiteral("quickPlayPath"), QString());
    variables.insert(QStringLiteral("quickPlaySingleplayer"), QString());
    variables.insert(QStringLiteral("quickPlayMultiplayer"), QString());
    variables.insert(QStringLiteral("quickPlayRealms"), QString());

    QStringList args;
    args << QStringLiteral("-Xms%1M").arg(qMax(256, minMemoryMb));
    args << QStringLiteral("-Xmx%1M").arg(qMax(minMemoryMb, maxMemoryMb));

    const QJsonObject argumentsObject = root.value(QStringLiteral("arguments")).toObject();
    if (!argumentsObject.isEmpty()) {
        args << collectArguments(argumentsObject.value(QStringLiteral("jvm")), variables);
    } else {
        args << QStringLiteral("-Djava.library.path=%1").arg(QDir::toNativeSeparators(nativeDir));
        args << QStringLiteral("-cp") << QDir::toNativeSeparators(classpath.join(classpathSeparator()));
    }

    const QString mainClass = root.value(QStringLiteral("mainClass")).toString();
    if (mainClass.isEmpty()) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Version %1 has no main class.").arg(instance.minecraftVersion.id);
        }
        return {};
    }
    args << mainClass;

    if (!argumentsObject.isEmpty()) {
        args << collectArguments(argumentsObject.value(QStringLiteral("game")), variables);
    } else {
        args << collectArguments(root.value(QStringLiteral("minecraftArguments")), variables);
    }

    plan.program = javaExecutable(javaPath);
    plan.arguments = args;
    plan.workingDirectory = instance.gamePath;
    plan.instanceId = instance.id;
    plan.versionId = instance.minecraftVersion.id;
    return plan;
}

QString MinecraftMetadata::manifestPath() const
{
    return Paths::metadataDir().filePath(QStringLiteral("version_manifest_v2.json"));
}

QString MinecraftMetadata::versionDir(const QString &versionId) const
{
    return Paths::versionsDir().filePath(versionId);
}

QString MinecraftMetadata::versionJsonPath(const QString &versionId) const
{
    return QDir(versionDir(versionId)).filePath(versionId + QStringLiteral(".json"));
}

QString MinecraftMetadata::clientJarPath(const QString &versionId) const
{
    return QDir(versionDir(versionId)).filePath(versionId + QStringLiteral(".jar"));
}

QString MinecraftMetadata::assetIndexPath(const QString &assetIndexId) const
{
    return QDir(Paths::assetsDir().filePath(QStringLiteral("indexes"))).filePath(assetIndexId + QStringLiteral(".json"));
}

QJsonObject MinecraftMetadata::readObject(const QString &path, QString *errorMessage) const
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Cannot read %1").arg(path);
        }
        return {};
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Invalid JSON in %1: %2").arg(path, parseError.errorString());
        }
        return {};
    }
    return document.object();
}

QJsonObject MinecraftMetadata::versionJson(const QString &versionId, QString *errorMessage) const
{
    return readObject(versionJsonPath(versionId), errorMessage);
}

QJsonObject MinecraftMetadata::resolvedVersionJson(const QString &versionId, QString *errorMessage) const
{
    const QJsonObject child = versionJson(versionId, errorMessage);
    if (child.isEmpty()) {
        return {};
    }

    const QString parentId = child.value(QStringLiteral("inheritsFrom")).toString();
    if (parentId.isEmpty()) {
        return child;
    }

    const QJsonObject parent = resolvedVersionJson(parentId, errorMessage);
    if (parent.isEmpty()) {
        return child;
    }
    return mergeInheritedVersion(parent, child);
}

QJsonObject MinecraftMetadata::mergeInheritedVersion(const QJsonObject &parent, const QJsonObject &child) const
{
    QJsonObject merged = parent;
    for (auto it = child.constBegin(); it != child.constEnd(); ++it) {
        if (it.key() == QStringLiteral("libraries")) {
            QJsonArray libraries = parent.value(QStringLiteral("libraries")).toArray();
            for (const QJsonValue &value : it.value().toArray()) {
                libraries.append(value);
            }
            merged.insert(it.key(), libraries);
        } else if (it.key() == QStringLiteral("arguments")) {
            QJsonObject arguments = parent.value(QStringLiteral("arguments")).toObject();
            const QJsonObject childArguments = it.value().toObject();
            for (const QString &key : childArguments.keys()) {
                QJsonArray list = arguments.value(key).toArray();
                for (const QJsonValue &value : childArguments.value(key).toArray()) {
                    list.append(value);
                }
                arguments.insert(key, list);
            }
            merged.insert(it.key(), arguments);
        } else {
            merged.insert(it.key(), it.value());
        }
    }
    return merged;
}

QString MinecraftMetadata::clientVersionId(const QJsonObject &root) const
{
    const QString parent = root.value(QStringLiteral("inheritsFrom")).toString();
    return parent.isEmpty() ? root.value(QStringLiteral("id")).toString() : parent;
}

QString MinecraftMetadata::offlineUuid(const QString &playerName) const
{
    const QByteArray seed = QStringLiteral("OfflinePlayer:%1").arg(playerName).toUtf8();
    QByteArray hash = QCryptographicHash::hash(seed, QCryptographicHash::Md5);
    hash[6] = static_cast<char>((hash[6] & 0x0f) | 0x30);
    hash[8] = static_cast<char>((hash[8] & 0x3f) | 0x80);
    return QUuid::fromRfc4122(hash).toString(QUuid::WithoutBraces);
}

bool MinecraftMetadata::rulesAllow(const QJsonArray &rules) const
{
    if (rules.isEmpty()) {
        return true;
    }

    bool allowed = false;
    for (const QJsonValue &value : rules) {
        const QJsonObject rule = value.toObject();
        if (!ruleMatches(rule)) {
            continue;
        }
        allowed = rule.value(QStringLiteral("action")).toString() == QStringLiteral("allow");
    }
    return allowed;
}

bool MinecraftMetadata::ruleMatches(const QJsonObject &rule) const
{
    const QJsonObject os = rule.value(QStringLiteral("os")).toObject();
    if (!os.isEmpty()) {
        const QString name = os.value(QStringLiteral("name")).toString();
        if (!name.isEmpty() && name != osName()) {
            return false;
        }
    }

    const QJsonObject features = rule.value(QStringLiteral("features")).toObject();
    if (!features.isEmpty()) {
        return false;
    }

    return true;
}

QStringList MinecraftMetadata::collectArguments(const QJsonValue &value, const QMap<QString, QString> &variables) const
{
    QStringList args;

    if (value.isString()) {
        for (const QString &part : QProcess::splitCommand(value.toString())) {
            args << replaceVariables(part, variables);
        }
        return args;
    }

    const QJsonArray array = value.toArray();
    for (const QJsonValue &entry : array) {
        if (entry.isString()) {
            args << replaceVariables(entry.toString(), variables);
            continue;
        }

        const QJsonObject object = entry.toObject();
        if (!rulesAllow(object.value(QStringLiteral("rules")).toArray())) {
            continue;
        }

        const QJsonValue argumentValue = object.value(QStringLiteral("value"));
        if (argumentValue.isString()) {
            args << replaceVariables(argumentValue.toString(), variables);
        } else {
            for (const QJsonValue &nested : argumentValue.toArray()) {
                args << replaceVariables(nested.toString(), variables);
            }
        }
    }
    return args;
}

QString MinecraftMetadata::replaceVariables(QString value, const QMap<QString, QString> &variables) const
{
    for (auto it = variables.constBegin(); it != variables.constEnd(); ++it) {
        value.replace(QStringLiteral("${%1}").arg(it.key()), it.value());
    }
    return value;
}

QString MinecraftMetadata::mavenPath(const QString &name) const
{
    const QStringList parts = name.split(QStringLiteral(":"));
    if (parts.size() < 3) {
        return {};
    }

    const QString groupPath = parts.at(0).split(QStringLiteral(".")).join(QStringLiteral("/"));
    const QString artifact = parts.at(1);
    const QString version = parts.at(2);
    QString classifier;
    QString extension = QStringLiteral("jar");

    if (parts.size() >= 4) {
        classifier = parts.at(3);
        if (classifier.contains(QStringLiteral("@"))) {
            const QStringList classifierParts = classifier.split(QStringLiteral("@"));
            classifier = classifierParts.at(0);
            extension = classifierParts.value(1, extension);
        }
    }

    QString fileName = artifact + QStringLiteral("-") + version;
    if (!classifier.isEmpty()) {
        fileName += QStringLiteral("-") + classifier;
    }
    fileName += QStringLiteral(".") + extension;
    return groupPath + QStringLiteral("/") + artifact + QStringLiteral("/") + version + QStringLiteral("/") + fileName;
}

QString MinecraftMetadata::javaExecutable(const QString &configuredJavaPath) const
{
    if (!configuredJavaPath.trimmed().isEmpty()) {
        return configuredJavaPath.trimmed();
    }

#if defined(Q_OS_WIN)
    const QString javaw = QStandardPaths::findExecutable(QStringLiteral("javaw.exe"));
    if (!javaw.isEmpty()) {
        return javaw;
    }
    const QString java = QStandardPaths::findExecutable(QStringLiteral("java.exe"));
#else
    const QString java = QStandardPaths::findExecutable(QStringLiteral("java"));
#endif
    return java.isEmpty() ? QStringLiteral("java") : java;
}

void MinecraftMetadata::collectLibraryDownloads(const QJsonObject &root, QList<DownloadRequest> &requests, QStringList *classpath, QStringList *nativeJars) const
{
    const QJsonArray libraries = root.value(QStringLiteral("libraries")).toArray();
    for (const QJsonValue &value : libraries) {
        const QJsonObject library = value.toObject();
        if (!rulesAllow(library.value(QStringLiteral("rules")).toArray())) {
            continue;
        }

        const QString name = library.value(QStringLiteral("name")).toString();
        const QJsonObject downloads = library.value(QStringLiteral("downloads")).toObject();
        const QJsonObject artifact = downloads.value(QStringLiteral("artifact")).toObject();
        if (!artifact.isEmpty()) {
            const QString path = artifact.value(QStringLiteral("path")).toString(mavenPath(name));
            const QString target = Paths::librariesDir().filePath(path);
            requests.append(requestFromJson(artifact, target, QStringLiteral("Library %1").arg(name)));
            if (classpath) {
                classpath->append(target);
            }
        } else if (!library.value(QStringLiteral("url")).toString().isEmpty()) {
            const QString path = mavenPath(name);
            const QString baseUrl = library.value(QStringLiteral("url")).toString();
            const QString target = Paths::librariesDir().filePath(path);
            DownloadRequest request;
            request.url = QUrl(baseUrl.endsWith(QStringLiteral("/")) ? baseUrl + path : baseUrl + QStringLiteral("/") + path);
            request.targetPath = target;
            request.sha1 = library.value(QStringLiteral("sha1")).toString();
            request.sha512 = library.value(QStringLiteral("sha512")).toString();
            request.expectedSize = static_cast<qint64>(library.value(QStringLiteral("size")).toDouble(-1));
            request.label = QStringLiteral("Library %1").arg(name);
            requests.append(request);
            if (classpath) {
                classpath->append(target);
            }
        }

        const QJsonObject natives = library.value(QStringLiteral("natives")).toObject();
        QString nativeClassifier = natives.value(osName()).toString();
        nativeClassifier.replace(QStringLiteral("${arch}"), QSysInfo::WordSize == 64 ? QStringLiteral("64") : QStringLiteral("32"));
        if (!nativeClassifier.isEmpty()) {
            const QJsonObject classifiers = downloads.value(QStringLiteral("classifiers")).toObject();
            const QJsonObject nativeDownload = classifiers.value(nativeClassifier).toObject();
            if (!nativeDownload.isEmpty()) {
                const QString nativePath = nativeDownload.value(QStringLiteral("path")).toString(mavenPath(name + QStringLiteral(":") + nativeClassifier));
                const QString target = Paths::librariesDir().filePath(nativePath);
                requests.append(requestFromJson(nativeDownload, target, QStringLiteral("Native %1").arg(name)));
                if (nativeJars) {
                    nativeJars->append(target);
                }
            }
        }
    }
}

bool MinecraftMetadata::collectAssetDownloads(const QJsonObject &root, QList<DownloadRequest> &requests, DownloadManager &downloads, QString *errorMessage)
{
    const QJsonObject assetIndex = root.value(QStringLiteral("assetIndex")).toObject();
    const QString id = assetIndex.value(QStringLiteral("id")).toString();
    if (id.isEmpty()) {
        return true;
    }

    QString error;
    const DownloadRequest indexRequest = requestFromJson(assetIndex, assetIndexPath(id), QStringLiteral("Asset index %1").arg(id));
    if (!downloads.downloadToFile(indexRequest, &error)) {
        if (errorMessage) {
            *errorMessage = error;
        }
        return false;
    }

    const QJsonObject indexRoot = readObject(assetIndexPath(id), &error);
    if (indexRoot.isEmpty()) {
        if (errorMessage) {
            *errorMessage = error;
        }
        return false;
    }
    const QJsonObject objects = indexRoot.value(QStringLiteral("objects")).toObject();
    for (auto it = objects.constBegin(); it != objects.constEnd(); ++it) {
        const QJsonObject asset = it.value().toObject();
        const QString hash = asset.value(QStringLiteral("hash")).toString();
        if (hash.size() < 2) {
            continue;
        }
        DownloadRequest request;
        request.url = QUrl(QStringLiteral("https://resources.download.minecraft.net/%1/%2").arg(hash.left(2), hash));
        request.targetPath = QDir(Paths::assetsDir().filePath(QStringLiteral("objects/%1").arg(hash.left(2)))).filePath(hash);
        request.sha1 = hash;
        request.expectedSize = static_cast<qint64>(asset.value(QStringLiteral("size")).toDouble(-1));
        request.label = QStringLiteral("Asset %1").arg(it.key());
        requests.append(request);
    }
    return true;
}

bool MinecraftMetadata::extractNatives(const QStringList &nativeJars, const QString &nativeDir, QString *errorMessage) const
{
    QDir dir(nativeDir);
    if (dir.exists()) {
        dir.removeRecursively();
    }
    QDir().mkpath(nativeDir);

    for (const QString &jar : nativeJars) {
#if defined(Q_OS_WIN)
        const QString script = QStringLiteral(
            "Add-Type -AssemblyName System.IO.Compression.FileSystem; "
            "$src=%1; $dst=%2; "
            "$zip=[System.IO.Compression.ZipFile]::OpenRead($src); "
            "try { foreach ($entry in $zip.Entries) { "
            "if ($entry.FullName.StartsWith('META-INF/') -or [string]::IsNullOrEmpty($entry.Name)) { continue } "
            "$target=Join-Path $dst $entry.FullName; "
            "$parent=Split-Path $target -Parent; "
            "if (!(Test-Path $parent)) { New-Item -ItemType Directory -Force -Path $parent | Out-Null } "
            "[System.IO.Compression.ZipFileExtensions]::ExtractToFile($entry, $target, $true); "
            "} } finally { $zip.Dispose() }"
        ).arg(quoteForPowerShell(QDir::toNativeSeparators(jar)), quoteForPowerShell(QDir::toNativeSeparators(nativeDir)));

        QProcess process;
        process.start(QStringLiteral("powershell.exe"), {
            QStringLiteral("-NoProfile"),
            QStringLiteral("-ExecutionPolicy"),
            QStringLiteral("Bypass"),
            QStringLiteral("-Command"),
            script,
        });
        if (!process.waitForFinished(120000) || process.exitCode() != 0) {
            if (errorMessage) {
                *errorMessage = QStringLiteral("Cannot extract natives from %1: %2").arg(jar, QString::fromLocal8Bit(process.readAllStandardError()));
            }
            return false;
        }
#else
        Q_UNUSED(jar)
        if (errorMessage) {
            *errorMessage = QStringLiteral("Native extraction is implemented for Windows in this build.");
        }
        return false;
#endif
    }

    const QFileInfoList metaInf = QDir(nativeDir).entryInfoList({QStringLiteral("META-INF")}, QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QFileInfo &entry : metaInf) {
        QDir(entry.absoluteFilePath()).removeRecursively();
    }
    return true;
}

} // namespace xylar
