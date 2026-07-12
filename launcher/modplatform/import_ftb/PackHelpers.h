#pragma once

#include <QIcon>
#include <QList>
#include <QMetaType>
#include <QString>
#include <QVariant>
#include "modplatform/ResourceAPI.h"

namespace FTBImportAPP {

struct Modpack {
    QString path;

    // json data
    QString uuid;
    int id;
    int versionId;
    QString name;
    QString version;
    QString mcVersion;
    int totalPlayTime;
    // not needed for instance creation
    QVariant jvmArgs;

    std::optional<ModPlatform::ModLoaderType> loaderType;
    QString loaderVersion;

    QIcon icon;
};

using ModpackList = QList<Modpack>;

Modpack parseDirectory(QString path);
void legacyInstanceParsing(QString path, std::optional<ModPlatform::ModLoaderType>* loaderType, QString* loaderVersion);
}  // namespace FTBImportAPP

// We need it for the proxy model
Q_DECLARE_METATYPE(FTBImportAPP::Modpack)
