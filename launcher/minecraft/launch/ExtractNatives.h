#pragma once

#include <launch/LaunchStep.h>

// FIXME: temporary wrapper for existing task.
class ExtractNatives : public LaunchStep {
    Q_OBJECT
   public:
    explicit ExtractNatives(LaunchTask* parent) : LaunchStep(parent) {};
    virtual ~ExtractNatives() {};

    void executeTask() override;
    bool canAbort() const override { return false; }
    void finalize() override;
};
