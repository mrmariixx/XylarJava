#include "modpacks/ModpackManager.h"

namespace xylar {

QStringList ModpackManager::supportedSources() const
{
    return {
        QStringLiteral("CurseForge"),
        QStringLiteral("FTB"),
        QStringLiteral("Modrinth import"),
        QStringLiteral("Prism-compatible export"),
    };
}

} // namespace xylar
