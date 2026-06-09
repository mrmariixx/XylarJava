#pragma once

#include <QList>
#include <QString>
#include <QStringList>

#include "download/DownloadManager.h"
#include "instances/Instance.h"

namespace xylar {

struct ModpackFile
{
    QString path;
    QStringList downloads;
    QString sha1;
    QString sha512;
    qint64 size = -1;
};

struct ModpackImportPlan
{
    QString name;
    QString summary;
    QString minecraftVersion;
    QString loaderName;
    QString extractedPath;
    QList<ModpackFile> files;
};

class ModpackManager
{
public:
    [[nodiscard]] QStringList supportedSources() const;
    [[nodiscard]] ModpackImportPlan readMrpack(const QString &path, QString *errorMessage = nullptr) const;
    [[nodiscard]] bool copyOverrides(const ModpackImportPlan &plan, const Instance &instance, QString *errorMessage = nullptr) const;

private:
    [[nodiscard]] bool extractArchive(const QString &archivePath, const QString &targetDir, QString *errorMessage) const;
    [[nodiscard]] bool copyDirectoryContents(const QString &sourceDir, const QString &targetDir, QString *errorMessage) const;
};

} // namespace xylar
