#pragma once

#include "tasks/Task.h"
namespace Java {

class SymlinkTask : public Task {
    Q_OBJECT
   public:
    SymlinkTask(QString final_path);
    virtual ~SymlinkTask() = default;

    void executeTask() override;

   protected:
    QString m_path;
    Task::Ptr m_task;
};
}  // namespace Java
