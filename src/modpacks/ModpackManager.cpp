#include "modpacks/ModpackManager.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>

#include "fs/Paths.h"

namespace xylar {
namespace {

QString quoteForPowerShell(QString value)
{
    value.replace(QStringLiteral("'"), QStringLiteral("''"));
    return QStringLiteral("'") + value + QStringLiteral("'");
}

QString loaderFromDependencies(const QJsonObject &dependencies)
{
    if (dependencies.contains(QStringLiteral("fabric-loader"))) {
        return QStringLiteral("Fabric");
    }
    if (dependencies.contains(QStringLiteral("quilt-loader"))) {
        return QStringLiteral("Quilt");
    }
    if (dependencies.contains(QStringLiteral("forge"))) {
        return QStringLiteral("Forge");
    }
    if (dependencies.contains(QStringLiteral("neoforge"))) {
        return QStringLiteral("NeoForge");
    }
    return QStringLiteral("Vanilla");
}

} // namespace

QStringList ModpackManager::supportedSources() const
{
    return {
        QStringLiteral("Modrinth .mrpack"),
        QStringLiteral("Local .jar mods"),
    };
}

ModpackImportPlan ModpackManager::readMrpack(const QString &path, QString *errorMessage) const
{
    ModpackImportPlan plan;
    const QFileInfo fileInfo(path);
    if (!fileInfo.exists()) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Pack file does not exist.");
        }
        return plan;
    }

    const QString extractRoot = Paths::metadataDir().filePath(QStringLiteral("modpack-imports/%1-%2")
        .arg(fileInfo.completeBaseName())
        .arg(QDateTime::currentMSecsSinceEpoch()));
    QDir().mkpath(extractRoot);
    if (!extractArchive(path, extractRoot, errorMessage)) {
        return plan;
    }

    QFile indexFile(QDir(extractRoot).filePath(QStringLiteral("modrinth.index.json")));
    if (!indexFile.open(QIODevice::ReadOnly)) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("modrinth.index.json was not found.");
        }
        return plan;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(indexFile.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Invalid modrinth.index.json: %1").arg(parseError.errorString());
        }
        return plan;
    }

    const QJsonObject root = document.object();
    if (root.value(QStringLiteral("game")).toString() != QStringLiteral("minecraft")) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("This .mrpack is not for Minecraft.");
        }
        return {};
    }

    const QJsonObject dependencies = root.value(QStringLiteral("dependencies")).toObject();
    plan.name = root.value(QStringLiteral("name")).toString(fileInfo.completeBaseName());
    plan.summary = root.value(QStringLiteral("summary")).toString();
    plan.minecraftVersion = dependencies.value(QStringLiteral("minecraft")).toString();
    plan.loaderName = loaderFromDependencies(dependencies);
    plan.extractedPath = extractRoot;

    const QJsonArray files = root.value(QStringLiteral("files")).toArray();
    for (const QJsonValue &value : files) {
        const QJsonObject item = value.toObject();
        const QJsonObject env = item.value(QStringLiteral("env")).toObject();
        if (env.value(QStringLiteral("client")).toString() == QStringLiteral("unsupported")) {
            continue;
        }

        ModpackFile packFile;
        packFile.path = item.value(QStringLiteral("path")).toString();
        packFile.size = static_cast<qint64>(item.value(QStringLiteral("fileSize")).toDouble(-1));
        const QJsonObject hashes = item.value(QStringLiteral("hashes")).toObject();
        packFile.sha1 = hashes.value(QStringLiteral("sha1")).toString();
        packFile.sha512 = hashes.value(QStringLiteral("sha512")).toString();
        for (const QJsonValue &download : item.value(QStringLiteral("downloads")).toArray()) {
            packFile.downloads.append(download.toString());
        }
        if (!packFile.path.isEmpty() && !packFile.downloads.isEmpty()) {
            plan.files.append(packFile);
        }
    }

    if (plan.minecraftVersion.isEmpty()) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("The pack has no Minecraft dependency.");
        }
        return {};
    }
    return plan;
}

bool ModpackManager::copyOverrides(const ModpackImportPlan &plan, const Instance &instance, QString *errorMessage) const
{
    const QString overrides = QDir(plan.extractedPath).filePath(QStringLiteral("overrides"));
    if (!QFileInfo::exists(overrides)) {
        return true;
    }
    return copyDirectoryContents(overrides, instance.gamePath, errorMessage);
}

bool ModpackManager::extractArchive(const QString &archivePath, const QString &targetDir, QString *errorMessage) const
{
#if defined(Q_OS_WIN)
    const QString script = QStringLiteral(
        "Add-Type -AssemblyName System.IO.Compression.FileSystem; "
        "[System.IO.Compression.ZipFile]::ExtractToDirectory(%1, %2);"
    ).arg(quoteForPowerShell(QDir::toNativeSeparators(archivePath)), quoteForPowerShell(QDir::toNativeSeparators(targetDir)));

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
            *errorMessage = QStringLiteral("Cannot extract pack: %1").arg(QString::fromLocal8Bit(process.readAllStandardError()));
        }
        return false;
    }
    return true;
#else
    Q_UNUSED(archivePath)
    Q_UNUSED(targetDir)
    if (errorMessage) {
        *errorMessage = QStringLiteral("Pack extraction is implemented for Windows in this build.");
    }
    return false;
#endif
}

bool ModpackManager::copyDirectoryContents(const QString &sourceDir, const QString &targetDir, QString *errorMessage) const
{
    QDir source(sourceDir);
    if (!source.exists()) {
        return true;
    }
    QDir().mkpath(targetDir);

    const QFileInfoList entries = source.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QFileInfo &entry : entries) {
        const QString target = QDir(targetDir).filePath(entry.fileName());
        if (entry.isDir()) {
            if (!copyDirectoryContents(entry.absoluteFilePath(), target, errorMessage)) {
                return false;
            }
        } else {
            QFile::remove(target);
            if (!QFile::copy(entry.absoluteFilePath(), target)) {
                if (errorMessage) {
                    *errorMessage = QStringLiteral("Cannot copy override %1").arg(entry.fileName());
                }
                return false;
            }
        }
    }
    return true;
}

} // namespace xylar
