#pragma once

#include <QJsonObject>
#include <QList>
#include <QMap>
#include <QString>

namespace ATLauncher {

enum class PackType { Public, Private };

enum class ModType {
    Root,
    Forge,
    Jar,
    Mods,
    Flan,
    Dependency,
    Ic2Lib,
    DenLib,
    Coremods,
    MCPC,
    Plugins,
    Extract,
    Decomp,
    TexturePack,
    ResourcePack,
    ShaderPack,
    TexturePackExtract,
    ResourcePackExtract,
    Millenaire,
    Unknown
};

enum class DownloadType { Server, Browser, Direct, Unknown };

struct VersionLoader {
    QString type;
    bool latest;
    bool recommended;
    bool choose;

    QString version;
};

struct VersionLibrary {
    QString url;
    QString file;
    QString server;
    QString md5;
    DownloadType download;
    QString download_raw;
};

struct VersionMod {
    QString name;
    QString version;
    QString url;
    QString file;
    QString md5;
    DownloadType download;
    QString download_raw;
    ModType type;
    QString type_raw;

    ModType extractTo;
    QString extractTo_raw;
    QString extractFolder;

    ModType decompType;
    QString decompType_raw;
    QString decompFile;

    QString description;
    bool optional;
    bool recommended;
    bool selected;
    bool hidden;
    bool library;
    QString group;
    QStringList depends;
    QString colour;
    QString warning;

    bool client;

    // computed
    bool effectively_hidden;
};

struct VersionConfigs {
    int filesize;
    QString sha1;
};

struct VersionMessages {
    QString install;
    QString update;
};

struct VersionKeep {
    QString base;
    QString target;
};

struct VersionKeeps {
    QList<VersionKeep> files;
    QList<VersionKeep> folders;
};

struct VersionDelete {
    QString base;
    QString target;
};

struct VersionDeletes {
    QList<VersionDelete> files;
    QList<VersionDelete> folders;
};

struct PackVersionMainClass {
    QString mainClass;
    QString depends;
};

struct PackVersionExtraArguments {
    QString arguments;
    QString depends;
};

struct PackVersion {
    QString version;
    QString minecraft;
    bool noConfigs;
    PackVersionMainClass mainClass;
    PackVersionExtraArguments extraArguments;

    VersionLoader loader;
    QList<VersionLibrary> libraries;
    QList<VersionMod> mods;
    VersionConfigs configs;

    QMap<QString, QString> colours;
    QMap<QString, QString> warnings;
    VersionMessages messages;

    VersionKeeps keeps;
    VersionDeletes deletes;
};

void loadVersion(PackVersion& v, QJsonObject& obj);

}  // namespace ATLauncher
