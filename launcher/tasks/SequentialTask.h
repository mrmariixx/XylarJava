#pragma once

#include "ConcurrentTask.h"

/** A concurrent task that only allows one concurrent task :)
 *
 *  This should be used when there's a need to maintain a strict ordering of task executions, and
 *  the starting of a task is contingent on the success of the previous one.
 *
 *  See MultipleOptionsTask if that's not the case.
 */
class SequentialTask : public ConcurrentTask {
    Q_OBJECT
   public:
    explicit SequentialTask(QString task_name = "");
    ~SequentialTask() override = default;

   protected slots:
    virtual void subTaskFailed(Task::Ptr, const QString& msg) override;

   protected:
    void updateState() override;
};
