#pragma once

#include <launch/LaunchStep.h>
#include <memory>

class ModMinecraftJar : public LaunchStep {
    Q_OBJECT
   public:
    explicit ModMinecraftJar(LaunchTask* parent) : LaunchStep(parent) {};
    virtual ~ModMinecraftJar() {};

    virtual void executeTask() override;
    virtual bool canAbort() const override { return false; }
    void finalize() override;

   private:
    bool removeJar();
};
