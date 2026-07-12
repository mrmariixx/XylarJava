#pragma once

#include <QMap>
#include <QString>
#include "net/NetJob.h"
#include "net/NetRequest.h"

struct AssetObject {
    QString getRelPath();
    QUrl getUrl();
    QString getLocalPath();
    Net::NetRequest::Ptr getDownloadAction();

    QString hash;
    qint64 size;
};

struct AssetsIndex {
    NetJob::Ptr getDownloadJob();

    QString id;
    QMap<QString, AssetObject> objects;
    bool isVirtual = false;
    bool mapToResources = false;
};

/// FIXME: this is absolutely horrendous. REDO!!!!
namespace AssetsUtils {
bool loadAssetsIndexJson(const QString& id, const QString& file, AssetsIndex& index);

QDir getAssetsDir(const QString& assetsId, const QString& resourcesFolder);

/// Reconstruct a virtual assets folder for the given assets ID and return the folder
bool reconstructAssets(QString assetsId, QString resourcesFolder);
}  // namespace AssetsUtils
