#include "minecraft/MinecraftVersion.h"

namespace xylar {

MinecraftVersion MinecraftVersion::latestReleasePlaceholder()
{
    return {
        QStringLiteral("latest-release"),
        QStringLiteral("release"),
        QStringLiteral("https://piston-meta.mojang.com/mc/game/version_manifest_v2.json"),
        {},
    };
}

} // namespace xylar
