#pragma once

#include <LoggedProcess.h>
#include <launch/LaunchStep.h>

class PostLaunchCommand : public LaunchStep {
    Q_OBJECT
   public:
    explicit PostLaunchCommand(LaunchTask* parent);
    virtual ~PostLaunchCommand() {};

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
