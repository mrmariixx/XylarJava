#include "MultipleOptionsTask.h"

#include <QDebug>

MultipleOptionsTask::MultipleOptionsTask(const QString& task_name) : ConcurrentTask(task_name, 1) {}

void MultipleOptionsTask::executeNextSubTask()
{
    if (m_done.size() != m_failed.size()) {
        emitSucceeded();
        return;
    }

    if (m_queue.isEmpty()) {
        emitFailed(tr("All attempts have failed!"));
        qWarning() << "All attempts have failed!";
        return;
    }

    ConcurrentTask::executeNextSubTask();
}

void MultipleOptionsTask::updateState()
{
    setProgress(m_done.count(), totalSize());
    setStatus(tr("Attempting task %1 out of %2").arg(QString::number(m_doing.count() + m_done.count()), QString::number(totalSize())));
}
