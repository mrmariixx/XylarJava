#pragma once

#include <QJsonObject>
#include <QList>
#include <QMap>
#include <QString>
#include <QUrl>
#include "modplatform/ModIndex.h"
#include "modplatform/ResourceType.h"

namespace Flame {
struct File {
    int projectId = 0;
    int fileId = 0;
    // NOTE: the opposite to 'optional'
    bool required = true;

    ModPlatform::IndexedPack pack;
    ModPlatform::IndexedVersion version;

    // our
    QString targetFolder = QStringLiteral("mods");
    ModPlatform::ResourceType resourceType;
};

struct Modloader {
    QString id;
    bool primary = false;
};

struct Minecraft {
    QString version;
    QString libraries;
    QList<Flame::Modloader> modLoaders;
    int recommendedRAM;
};

struct Manifest {
    QString manifestType;
    int manifestVersion = 0;
    Flame::Minecraft minecraft;
    QString name;
    QString version;
    QString author;
    // File id -> File
    QMap<int, Flame::File> files;
    QString overrides;

    bool is_loaded = false;
};

void loadManifest(Flame::Manifest& m, const QString& filepath);
}  // namespace Flame
