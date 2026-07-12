#pragma once

#include <launch/LaunchStep.h>
#include "minecraft/auth/AuthSession.h"
#include "minecraft/launch/MinecraftTarget.h"

// FIXME: temporary wrapper for existing task.
class PrintInstanceInfo : public LaunchStep {
    Q_OBJECT
   public:
    explicit PrintInstanceInfo(LaunchTask* parent, AuthSessionPtr session, MinecraftTarget::Ptr targetToJoin)
        : LaunchStep(parent), m_session(session), m_targetToJoin(targetToJoin) {};
    virtual ~PrintInstanceInfo() = default;

    virtual void executeTask();
    virtual bool canAbort() const { return false; }

   private:
    AuthSessionPtr m_session;
    MinecraftTarget::Ptr m_targetToJoin;
};
