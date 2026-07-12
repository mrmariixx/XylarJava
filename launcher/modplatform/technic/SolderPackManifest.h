#pragma once

#include <QJsonObject>
#include <QList>
#include <QString>

namespace TechnicSolder {

struct Pack {
    QString recommended;
    QString latest;
    QList<QString> builds;
};

void loadPack(Pack& v, QJsonObject& obj);

struct PackBuildMod {
    QString name;
    QString version;
    QString md5;
    QString url;
};

struct PackBuild {
    QString minecraft;
    QList<PackBuildMod> mods;
};

void loadPackBuild(PackBuild& v, QJsonObject& obj);

}  // namespace TechnicSolder
