#include <QFile>

#include "BaseInstaller.h"
#include "FileSystem.h"
#include "minecraft/MinecraftInstance.h"

BaseInstaller::BaseInstaller() {}

bool BaseInstaller::isApplied(MinecraftInstance* on)
{
    return QFile::exists(filename(on->instanceRoot()));
}

bool BaseInstaller::add(MinecraftInstance* to)
{
    if (!patchesDir(to->instanceRoot()).exists()) {
        QDir(to->instanceRoot()).mkdir("patches");
    }

    if (isApplied(to)) {
        if (!remove(to)) {
            return false;
        }
    }

    return true;
}

bool BaseInstaller::remove(MinecraftInstance* from)
{
    return FS::deletePath(filename(from->instanceRoot()));
}

QString BaseInstaller::filename(const QString& root) const
{
    return patchesDir(root).absoluteFilePath(id() + ".json");
}
QDir BaseInstaller::patchesDir(const QString& root) const
{
    return QDir(root + "/patches/");
}
