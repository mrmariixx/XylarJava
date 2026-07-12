#include "PreLaunchCommand.h"
#include <launch/LaunchTask.h>

PreLaunchCommand::PreLaunchCommand(LaunchTask* parent) : LaunchStep(parent)
{
    auto instance = m_parent->instance();
    m_command = instance->getPreLaunchCommand();
    m_process.setProcessEnvironment(instance->createEnvironment());
    connect(&m_process, &LoggedProcess::log, this, &PreLaunchCommand::logLines);
    connect(&m_process, &LoggedProcess::stateChanged, this, &PreLaunchCommand::on_state);
}

void PreLaunchCommand::executeTask()
{
    auto cmd = m_parent->substituteVariables(m_command);
    emit logLine(tr("Running Pre-Launch command: %1").arg(cmd), MessageLevel::Launcher);
    auto args = QProcess::splitCommand(cmd);
    const QString program = args.takeFirst();
    m_process.start(program, args);
}

void PreLaunchCommand::on_state(LoggedProcess::State state)
{
    auto getError = [this]() { return tr("Pre-Launch command failed with code %1.\n\n").arg(m_process.exitCode()); };
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
                emit logLine(tr("Pre-Launch command ran successfully.\n\n"), MessageLevel::Launcher);
                emitSucceeded();
            }
        }
        default:
            break;
    }
}

void PreLaunchCommand::setWorkingDirectory(const QString& wd)
{
    m_process.setWorkingDirectory(wd);
}

bool PreLaunchCommand::abort()
{
    auto state = m_process.state();
    if (state == LoggedProcess::Running || state == LoggedProcess::Starting) {
        m_process.kill();
    }
    return true;
}
