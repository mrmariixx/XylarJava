#include <QObject>

#include "LocalResourceParse.h"

#include "LocalDataPackParseTask.h"
#include "LocalModParseTask.h"
#include "LocalShaderPackParseTask.h"
#include "LocalTexturePackParseTask.h"
#include "LocalWorldSaveParseTask.h"
#include "modplatform/ResourceType.h"

namespace ResourceUtils {
ModPlatform::ResourceType identify(QFileInfo file)
{
    if (file.exists() && file.isFile()) {
        if (ModUtils::validate(file)) {
            // mods can contain resource and data packs so they must be tested first
            qDebug() << file.fileName() << "is a mod";
            return ModPlatform::ResourceType::Mod;
        } else if (DataPackUtils::validateResourcePack(file)) {
            qDebug() << file.fileName() << "is a resource pack";
            return ModPlatform::ResourceType::ResourcePack;
        } else if (TexturePackUtils::validate(file)) {
            qDebug() << file.fileName() << "is a pre 1.6 texture pack";
            return ModPlatform::ResourceType::TexturePack;
        } else if (DataPackUtils::validate(file)) {
            qDebug() << file.fileName() << "is a data pack";
            return ModPlatform::ResourceType::DataPack;
        } else if (WorldSaveUtils::validate(file)) {
            qDebug() << file.fileName() << "is a world save";
            return ModPlatform::ResourceType::World;
        } else if (ShaderPackUtils::validate(file)) {
            qDebug() << file.fileName() << "is a shader pack";
            return ModPlatform::ResourceType::ShaderPack;
        } else {
            qDebug() << "Can't Identify" << file.fileName();
        }
    } else {
        qDebug() << "Can't find" << file.absolutePath();
    }
    return ModPlatform::ResourceType::Unknown;
}

}  // namespace ResourceUtils
