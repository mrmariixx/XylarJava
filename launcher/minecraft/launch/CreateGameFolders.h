#pragma once

#include <LoggedProcess.h>
#include <launch/LaunchStep.h>
#include <minecraft/auth/AuthSession.h>

// Create the main .minecraft for the instance and any other necessary folders
class CreateGameFolders : public LaunchStep {
    Q_OBJECT
   public:
    explicit CreateGameFolders(LaunchTask* parent);
    virtual ~CreateGameFolders() {};

    virtual void executeTask();
    virtual bool canAbort() const { return false; }
};
