#pragma once

#include <QString>

#include "minecraft/MinecraftVersion.h"

namespace xylar {

struct Instance
{
    QString id;
    QString name;
    QString loader;
    QString rootPath;
    QString gamePath;
    qint64 lastLaunch = 0;
    MinecraftVersion minecraftVersion;

    [[nodiscard]] bool isValid() const;
};

} // namespace xylar
