#pragma once

#include <QHash>
#include <QQueue>
#include <QSet>
#include <QUuid>
#include <memory>

#include "tasks/Task.h"

/*!
 * Runs a list of tasks concurrently (according to `max_concurrent` parameter).
 * Behaviour is the same as regular Task (e.g. starts using start())
 */
class ConcurrentTask : public Task {
    Q_OBJECT
   public:
    using Ptr = shared_qobject_ptr<ConcurrentTask>;

    explicit ConcurrentTask(QString task_name = "", int max_concurrent = 6);
    ~ConcurrentTask() override;

    // safe to call before starting the task
    void setMaxConcurrent(int max_concurrent) { m_total_max_size = max_concurrent; }

    bool canAbort() const override { return true; }

    inline auto isMultiStep() const -> bool override { return totalSize() > 1; }
    auto getStepProgress() const -> TaskStepProgressList override;

    //! Adds a task to execute in this ConcurrentTask
    void addTask(Task::Ptr task);

   public slots:
    bool abort() override;

    /** Resets the internal state of the task.
     *  This allows the same task to be re-used.
     */
    void clear();

   protected slots:
    void executeTask() override;

    virtual void executeNextSubTask();

    void subTaskSucceeded(Task::Ptr);
    virtual void subTaskFailed(Task::Ptr, const QString& msg);
    void subTaskFinished(Task::Ptr, TaskStepState);
    void subTaskStatus(Task::Ptr task, const QString& msg);
    void subTaskDetails(Task::Ptr task, const QString& msg);
    void subTaskProgress(Task::Ptr task, qint64 current, qint64 total);

   protected:
    // NOTE: This is not thread-safe.
    unsigned int totalSize() const { return static_cast<unsigned int>(m_queue.size() + m_doing.size() + m_done.size()); }

    virtual void updateState();

    void startSubTask(Task::Ptr task);

   protected:
    QQueue<Task::Ptr> m_queue;

    QHash<Task*, Task::Ptr> m_doing;
    QHash<Task*, Task::Ptr> m_done;
    QHash<Task*, Task::Ptr> m_failed;
    QHash<Task*, Task::Ptr> m_succeeded;

    QHash<QUuid, std::shared_ptr<TaskStepProgress>> m_task_progress;

    int m_total_max_size;
};
