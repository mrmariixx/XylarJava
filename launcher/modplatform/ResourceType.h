#pragma once

#include <set>

#include <QDebug>
#include <QFileInfo>
#include <QObject>

namespace ModPlatform {

enum class ResourceType { Mod, ResourcePack, ShaderPack, Modpack, DataPack, World, Screenshots, TexturePack, Unknown };

namespace ResourceTypeUtils {
static const std::set<ResourceType> VALID_RESOURCES = { ResourceType::DataPack,   ResourceType::ResourcePack, ResourceType::TexturePack,
                                                        ResourceType::ShaderPack, ResourceType::World,        ResourceType::Mod };
QString getName(ResourceType type);
}  // namespace ResourceTypeUtils
}  // namespace ModPlatform
