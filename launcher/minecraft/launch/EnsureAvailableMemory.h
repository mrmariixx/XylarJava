#pragma once

#include "launch/LaunchStep.h"
#include "minecraft/MinecraftInstance.h"

class EnsureAvailableMemory : public LaunchStep {
    Q_OBJECT

   public:
    explicit EnsureAvailableMemory(LaunchTask* parent, MinecraftInstance* instance);
    ~EnsureAvailableMemory() override = default;

    void executeTask() override;
    bool canAbort() const override { return false; }

   private:
    MinecraftInstance* m_instance;
};
