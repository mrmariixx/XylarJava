#include "PostLaunchCommand.h"
#include <launch/LaunchTask.h>

PostLaunchCommand::PostLaunchCommand(LaunchTask* parent) : LaunchStep(parent)
{
    auto instance = m_parent->instance();
    m_command = instance->getPostExitCommand();
    m_process.setProcessEnvironment(instance->createEnvironment());
    connect(&m_process, &LoggedProcess::log, this, &PostLaunchCommand::logLines);
    connect(&m_process, &LoggedProcess::stateChanged, this, &PostLaunchCommand::on_state);
}

void PostLaunchCommand::executeTask()
{
    auto cmd = m_parent->substituteVariables(m_command);
    emit logLine(tr("Running Post-Launch command: %1").arg(cmd), MessageLevel::Launcher);
    auto args = QProcess::splitCommand(cmd);

    const QString program = args.takeFirst();
    m_process.start(program, args);
}

void PostLaunchCommand::on_state(LoggedProcess::State state)
{
    auto getError = [this]() { return tr("Post-Launch command failed with code %1.\n\n").arg(m_process.exitCode()); };
    switch (state) {
        case LoggedProcess::Aborted:
        case LoggedProcess::Crashed:
        case LoggedProcess::FailedToStart: {
            auto error = getError();
            emit logLine(error, MessageLevel::Fatal);
            emitFailed(error);
            return;
        }
        case LoggedProcess::Finished: {
            if (m_process.exitCode() != 0) {
                auto error = getError();
                emit logLine(error, MessageLevel::Fatal);
                emitFailed(error);
            } else {
                emit logLine(tr("Post-Launch command ran successfully.\n\n"), MessageLevel::Launcher);
                emitSucceeded();
            }
        }
        default:
            break;
    }
}

void PostLaunchCommand::setWorkingDirectory(const QString& wd)
{
    m_process.setWorkingDirectory(wd);
}

bool PostLaunchCommand::abort()
{
    auto state = m_process.state();
    if (state == LoggedProcess::Running || state == LoggedProcess::Starting) {
        m_process.kill();
    }
    return true;
}
