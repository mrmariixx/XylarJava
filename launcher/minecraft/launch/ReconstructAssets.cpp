#include "ReconstructAssets.h"
#include "launch/LaunchTask.h"
#include "minecraft/AssetsUtils.h"
#include "minecraft/MinecraftInstance.h"
#include "minecraft/PackProfile.h"

void ReconstructAssets::executeTask()
{
    auto instance = m_parent->instance();
    auto components = instance->getPackProfile();
    auto profile = components->getProfile();
    auto assets = profile->getMinecraftAssets();

    if (!AssetsUtils::reconstructAssets(assets->id, instance->resourcesDir())) {
        emit logLine("Failed to reconstruct Minecraft assets.", MessageLevel::Error);
    }

    emitSucceeded();
}
