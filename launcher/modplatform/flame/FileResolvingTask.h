#pragma once

#include "PackManifest.h"
#include "tasks/Task.h"

namespace Flame {
class FileResolvingTask : public Task {
    Q_OBJECT
   public:
    explicit FileResolvingTask(Flame::Manifest& toProcess);
    virtual ~FileResolvingTask() = default;

    bool canAbort() const override { return true; }
    bool abort() override;

    const Flame::Manifest& getResults() const { return m_manifest; }

   protected:
    virtual void executeTask() override;

   protected slots:
    void netJobFinished(QByteArray* response);

   private:
    void getFlameProjects();

   private: /* data */
    Flame::Manifest m_manifest;
    Task::Ptr m_task;
};
}  // namespace Flame
