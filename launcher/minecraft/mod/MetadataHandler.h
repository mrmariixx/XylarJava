#pragma once

#include "modplatform/packwiz/Packwiz.h"

namespace Metadata {
using ModStruct = Packwiz::V1::Mod;

inline ModStruct create(const QDir& index_dir, ModPlatform::IndexedPack& mod_pack, ModPlatform::IndexedVersion& mod_version)
{
    return Packwiz::V1::createModFormat(index_dir, mod_pack, mod_version);
}

inline void update(const QDir& index_dir, ModStruct& mod)
{
    Packwiz::V1::updateModIndex(index_dir, mod);
}

inline void remove(const QDir& index_dir, QString mod_slug)
{
    Packwiz::V1::deleteModIndex(index_dir, mod_slug);
}

inline ModStruct get(const QDir& index_dir, QString mod_slug)
{
    return Packwiz::V1::getIndexForMod(index_dir, std::move(mod_slug));
}

inline ModStruct get(const QDir& index_dir, QVariant& mod_id)
{
    return Packwiz::V1::getIndexForMod(index_dir, mod_id);
}

};  // namespace Metadata
