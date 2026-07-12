#include "Task.h"

#include <QDebug>

#include "AssertHelpers.h"

Q_LOGGING_CATEGORY(taskLogC, "launcher.task")

Task::Task(bool show_debug) : m_show_debug(show_debug)
{
    m_uid = QUuid::createUuid();
    setAutoDelete(false);
}

void Task::setStatus(const QString& new_status)
{
    if (m_status != new_status) {
        m_status = new_status;
        emit status(m_status);
    }
}

void Task::setDetails(const QString& new_details)
{
    if (m_details != new_details) {
        m_details = new_details;
        emit details(m_details);
    }
}

void Task::setProgress(qint64 current, qint64 total)
{
    if ((m_progress != current) || (m_progressTotal != total)) {
        m_progress = current;
        m_progressTotal = total;

        emit progress(m_progress, m_progressTotal);
    }
}

void Task::start()
{
    switch (m_state) {
        case State::Inactive: {
            if (m_show_debug)
                qCDebug(taskLogC) << "Task" << describe() << "starting for the first time";
            break;
        }
        case State::AbortedByUser: {
            if (m_show_debug)
                qCDebug(taskLogC) << "Task" << describe() << "restarting for after being aborted by user";
            break;
        }
        case State::Failed: {
            if (m_show_debug)
                qCDebug(taskLogC) << "Task" << describe() << "restarting for after failing at first";
            break;
        }
        case State::Succeeded: {
            if (m_show_debug)
                qCDebug(taskLogC) << "Task" << describe() << "restarting for after succeeding at first";
            break;
        }
        case State::Running: {
            if (ASSERT_NEVER(isRunning()) && m_show_debug)
                qCWarning(taskLogC) << "The launcher tried to start task" << describe() << "while it was already running!";
            return;
        }
    }
    // NOTE: only fall through to here in end states
    m_state = State::Running;
    emit started();
    executeTask();
}

void Task::emitFailed(QString reason)
{
    // Don't fail twice.
    if (ASSERT_NEVER(!isRunning())) {
        qCCritical(taskLogC) << "Task" << describe() << "failed while not running!!!!:" << reason;
        return;
    }
    m_state = State::Failed;
    m_failReason = reason;
    qCCritical(taskLogC) << "Task" << describe() << "failed:" << reason;
    emit failed(reason);
    emit finished();
}

void Task::emitAborted()
{
    // Don't abort twice.
    if (ASSERT_NEVER(!isRunning())) {
        qCCritical(taskLogC) << "Task" << describe() << "aborted while not running!!!!";
        return;
    }
    m_state = State::AbortedByUser;
    m_failReason = tr("Aborted");
    if (m_show_debug)
        qCDebug(taskLogC) << "Task" << describe() << "aborted.";
    emit aborted();
    emit finished();
}

void Task::emitSucceeded()
{
    // Don't succeed twice.
    if (ASSERT_NEVER(!isRunning())) {
        qCCritical(taskLogC) << "Task" << describe() << "succeeded while not running!!!!";
        return;
    }
    m_state = State::Succeeded;
    if (m_show_debug)
        qCDebug(taskLogC) << "Task" << describe() << "succeeded";
    emit succeeded();
    emit finished();
}

void Task::propagateStepProgress(TaskStepProgress const& task_progress)
{
    emit stepProgress(task_progress);
}

QString Task::describe()
{
    QString outStr;
    QTextStream out(&outStr);
    out << metaObject()->className() << QChar('(');
    auto name = objectName();
    if (name.isEmpty()) {
        out << QString("0x%1").arg(reinterpret_cast<quintptr>(this), 0, 16);
    } else {
        out << name;
    }
    out << " ID: " << m_uid.toString(QUuid::WithoutBraces);
    out << QChar(')');
    out.flush();
    return outStr;
}

bool Task::isRunning() const
{
    return m_state == State::Running;
}

bool Task::isFinished() const
{
    return m_state != State::Running && m_state != State::Inactive;
}

bool Task::wasSuccessful() const
{
    return m_state == State::Succeeded;
}

QString Task::failReason() const
{
    return m_failReason;
}

void Task::propagateFromOther(Task* other)
{
    Q_ASSERT(other);
    connect(other, &Task::status, this, &Task::setStatus);
    connect(other, &Task::details, this, &Task::setDetails);
    connect(other, &Task::progress, this, &Task::setProgress);
    connect(other, &Task::stepProgress, this, &Task::propagateStepProgress);

    setStatus(other->getStatus());
    setDetails(other->getDetails());
    setProgress(other->getProgress(), other->getTotalProgress());
    for (const auto& progress : other->getStepProgress()) {
        propagateStepProgress(*progress);
    }
}

void Task::logWarning(const QString& line)
{
    qWarning() << line;
    m_Warnings.append(line);

    emit warningLogged(line);
}

QStringList Task::warnings() const
{
    return m_Warnings;
}
