#pragma once

#include <launch/LaunchStep.h>
#include <memory>

class ReconstructAssets : public LaunchStep {
    Q_OBJECT
   public:
    explicit ReconstructAssets(LaunchTask* parent) : LaunchStep(parent) {};
    virtual ~ReconstructAssets() {};

    void executeTask() override;
    bool canAbort() const override { return false; }
};
