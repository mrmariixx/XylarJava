#pragma once

#include "modplatform/ModIndex.h"

#include <QString>
#include <QUrl>
#include <QVariant>

class QDir;

namespace Packwiz {

class V1 {
   public:
    // can also represent other resources beside loader mods - but this is what packwiz calls it
    struct Mod {
        QString slug{};
        QString name{};
        QString filename{};
        ModPlatform::Side side{ ModPlatform::Side::UniversalSide };
        ModPlatform::ModLoaderTypes loaders;
        QStringList mcVersions;
        ModPlatform::IndexedVersionType releaseType;

        // [download]
        QString mode{};
        QUrl url{};
        QString hash_format{};
        QString hash{};

        // [update]
        ModPlatform::ResourceProvider provider{};
        QVariant file_id{};
        QVariant project_id{};
        QString version_number{};

        QList<ModPlatform::Dependency> dependencies;

       public:
        // This is a totally heuristic, but should work for now.
        auto isValid() const -> bool { return !slug.isEmpty() && !project_id.isNull(); }

        // Different providers can use different names for the same thing
        // Modrinth-specific
        auto mod_id() -> QVariant& { return project_id; }
        auto version() -> QVariant& { return file_id; }
    };

    /* Generates the object representing the information in a mod.pw.toml file via
     * its common representation in the launcher, when downloading mods.
     * */
    static auto createModFormat(const QDir& index_dir, ModPlatform::IndexedPack& mod_pack, ModPlatform::IndexedVersion& mod_version) -> Mod;

    /* Updates the mod index for the provided mod.
     * This creates a new index if one does not exist already
     * TODO: Ask the user if they want to override, and delete the old mod's files, or keep the old one.
     * */
    static void updateModIndex(const QDir& index_dir, Mod& mod);

    /* Deletes the metadata for the mod with the given slug. If the metadata doesn't exist, it does nothing. */
    static void deleteModIndex(const QDir& index_dir, QString& mod_slug);

    /* Gets the metadata for a mod with a particular file name.
     * If the mod doesn't have a metadata, it simply returns an empty Mod object.
     * */
    static auto getIndexForMod(const QDir& index_dir, QString slug) -> Mod;

    /* Gets the metadata for a mod with a particular id.
     * If the mod doesn't have a metadata, it simply returns an empty Mod object.
     * */
    static auto getIndexForMod(const QDir& index_dir, QVariant& mod_id) -> Mod;
};

}  // namespace Packwiz
