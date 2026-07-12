#pragma once

#include "launch/LaunchStep.h"
#include "minecraft/MinecraftInstance.h"

class EnsureOfflineLibraries : public LaunchStep {
    Q_OBJECT

   public:
    explicit EnsureOfflineLibraries(LaunchTask* parent, MinecraftInstance* instance);
    ~EnsureOfflineLibraries() override = default;

    void executeTask() override;
    bool canAbort() const override { return false; }

   private:
    MinecraftInstance* m_instance;
};
