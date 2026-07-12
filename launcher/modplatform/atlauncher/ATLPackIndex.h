#pragma once

#include "ATLPackManifest.h"

#include <QList>
#include <QMetaType>
#include <QString>

namespace ATLauncher {

struct IndexedVersion {
    QString version;
    QString minecraft;
};

struct IndexedPack {
    int id;
    int position;
    QString name;
    PackType type;
    QList<IndexedVersion> versions;
    bool system;
    QString description;

    QString safeName;
};

void loadIndexedPack(IndexedPack& m, QJsonObject& obj);
}  // namespace ATLauncher

Q_DECLARE_METATYPE(ATLauncher::IndexedPack)
Q_DECLARE_METATYPE(QList<ATLauncher::IndexedVersion>)
