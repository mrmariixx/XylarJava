#pragma once

#include <LoggedProcess.h>
#include <QObjectPtr.h>
#include <java/JavaChecker.h>
#include <launch/LaunchStep.h>
#include <net/Mode.h>

class TaskStepWrapper : public LaunchStep {
    Q_OBJECT
   public:
    explicit TaskStepWrapper(LaunchTask* parent, Task::Ptr task) : LaunchStep(parent), m_task(task) {};
    virtual ~TaskStepWrapper() = default;

    void executeTask() override;
    bool canAbort() const override;
    void proceed() override;
   public slots:
    bool abort() override;

   private slots:
    void updateFinished();

   private:
    Task::Ptr m_task;
};
