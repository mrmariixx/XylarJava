#include "SequentialTask.h"

#include <QDebug>
#include "tasks/ConcurrentTask.h"

SequentialTask::SequentialTask(QString task_name) : ConcurrentTask(task_name, 1) {}

void SequentialTask::subTaskFailed(Task::Ptr task, const QString& msg)
{
    qWarning() << msg;
    ConcurrentTask::subTaskFailed(task, msg);
    emitFailed(msg);
}

void SequentialTask::updateState()
{
    setProgress(m_done.count(), totalSize());
    setStatus(tr("Executing task %1 out of %2").arg(QString::number(m_doing.count() + m_done.count()), QString::number(totalSize())));
}
