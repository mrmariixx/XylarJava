#include "ResourceType.h"

namespace ModPlatform {
static const QMap<ResourceType, QString> s_packedTypeNames = { { ResourceType::ResourcePack, QObject::tr("resource pack") },
                                                               { ResourceType::TexturePack, QObject::tr("texture pack") },
                                                               { ResourceType::DataPack, QObject::tr("data pack") },
                                                               { ResourceType::ShaderPack, QObject::tr("shader pack") },
                                                               { ResourceType::World, QObject::tr("world save") },
                                                               { ResourceType::Mod, QObject::tr("mod") },
                                                               { ResourceType::Unknown, QObject::tr("unknown") } };

namespace ResourceTypeUtils {

QString getName(ResourceType type)
{
    return s_packedTypeNames.constFind(type).value();
}

}  // namespace ResourceTypeUtils
}  // namespace ModPlatform
