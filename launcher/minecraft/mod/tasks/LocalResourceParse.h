#pragma once

#include <QFileInfo>
#include "modplatform/ResourceType.h"

namespace ResourceUtils {
ModPlatform::ResourceType identify(QFileInfo file);
}  // namespace ResourceUtils
