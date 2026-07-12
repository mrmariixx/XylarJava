#pragma once

#include "modplatform/ModIndex.h"

#include "BaseInstance.h"

namespace Modrinth {

void loadIndexedPack(ModPlatform::IndexedPack& pack, QJsonObject& obj);
void loadExtraPackData(ModPlatform::IndexedPack& pack, QJsonObject& obj);
auto loadIndexedPackVersion(QJsonObject& obj, const QString& preferred_hash_type = "sha512", const QString& preferred_file_name = "")
    -> ModPlatform::IndexedVersion;

}  // namespace Modrinth
