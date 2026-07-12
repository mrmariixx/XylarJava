#pragma once

#include <memory>

#include "BaseVersion.h"

class MinecraftInstance;
class QDir;
class QString;
class QObject;
class Task;
class BaseVersion;

class BaseInstaller {
   public:
    BaseInstaller();
    virtual ~BaseInstaller() {};
    bool isApplied(MinecraftInstance* on);

    virtual bool add(MinecraftInstance* to);
    virtual bool remove(MinecraftInstance* from);

    virtual Task* createInstallTask(MinecraftInstance* instance, BaseVersion::Ptr version, QObject* parent) = 0;

   protected:
    virtual QString id() const = 0;
    QString filename(const QString& root) const;
    QDir patchesDir(const QString& root) const;
};
