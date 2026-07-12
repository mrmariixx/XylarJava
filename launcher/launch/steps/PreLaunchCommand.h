#pragma once

#include "LoggedProcess.h"
#include "launch/LaunchStep.h"

class PreLaunchCommand : public LaunchStep {
    Q_OBJECT
   public:
    explicit PreLaunchCommand(LaunchTask* parent);
    virtual ~PreLaunchCommand() {};

    virtual void executeTask();
    virtual bool abort();
    virtual bool canAbort() const { return true; }
    void setWorkingDirectory(const QString& wd);
   private slots:
    void on_state(LoggedProcess::State state);

   private:
    LoggedProcess m_process;
    QString m_command;
};
