#include "TaskStepWrapper.h"
#include "tasks/Task.h"

void TaskStepWrapper::executeTask()
{
    if (m_state == Task::State::AbortedByUser) {
        emitFailed(tr("Task aborted."));
        return;
    }
    connect(m_task.get(), &Task::finished, this, &TaskStepWrapper::updateFinished);
    propagateFromOther(m_task.get());
    emit progressReportingRequest();
}

void TaskStepWrapper::proceed()
{
    m_task->start();
}

void TaskStepWrapper::updateFinished()
{
    if (m_task->wasSuccessful()) {
        m_task.reset();
        emitSucceeded();
    } else {
        QString reason = tr("Instance update failed because: %1\n\n").arg(m_task->failReason());
        m_task.reset();
        emit logLine(reason, MessageLevel::Fatal);
        emitFailed(reason);
    }
}

bool TaskStepWrapper::canAbort() const
{
    if (m_task) {
        return m_task->canAbort();
    }
    return true;
}

bool TaskStepWrapper::abort()
{
    if (m_task && m_task->canAbort()) {
        return m_task->abort();
    }
    return Task::abort();
}
