#pragma once

#include <QList>
#include <QMetaType>
#include <QString>

namespace Technic {
struct Modpack {
    QString slug;

    QString name;
    QString logoUrl;
    QString logoName;

    bool broken = true;

    QString url;
    bool isSolder = false;
    QString minecraftVersion;

    bool metadataLoaded = false;
    QString websiteUrl;
    QString author;
    QString description;
    QString currentVersion;

    bool versionsLoaded = false;
    QString recommended;
    QList<QString> versions;
};
}  // namespace Technic

Q_DECLARE_METATYPE(Technic::Modpack)
