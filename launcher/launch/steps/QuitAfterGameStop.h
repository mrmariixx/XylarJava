#pragma once

#include <launch/LaunchStep.h>

class QuitAfterGameStop : public LaunchStep {
    Q_OBJECT
   public:
    explicit QuitAfterGameStop(LaunchTask* parent) : LaunchStep(parent) {};
    virtual ~QuitAfterGameStop() = default;

    virtual void executeTask();
    virtual bool canAbort() const { return false; }
};
