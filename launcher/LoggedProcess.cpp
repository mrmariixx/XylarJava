#include "LoggedProcess.h"
#include <QDebug>
#include <QStringDecoder>
#include "MessageLevel.h"

LoggedProcess::LoggedProcess(const QStringConverter::Encoding output_codec, QObject* parent)
    : QProcess(parent), m_err_decoder(output_codec), m_out_decoder(output_codec)
{
    // QProcess has a strange interface... let's map a lot of those into a few.
    connect(this, &QProcess::readyReadStandardOutput, this, &LoggedProcess::on_stdOut);
    connect(this, &QProcess::readyReadStandardError, this, &LoggedProcess::on_stdErr);
    connect(this, &QProcess::finished, this, &LoggedProcess::on_exit);
    connect(this, &QProcess::errorOccurred, this, &LoggedProcess::on_error);
    connect(this, &QProcess::stateChanged, this, &LoggedProcess::on_stateChange);
}

LoggedProcess::~LoggedProcess()
{
    if (m_is_detachable) {
        setProcessState(QProcess::NotRunning);
    }
}

QStringList LoggedProcess::reprocess(const QByteArray& data, QStringDecoder& decoder)
{
    QString str = decoder(data);

    if (!m_leftover_line.isEmpty()) {
        str.prepend(m_leftover_line);
        m_leftover_line = "";
    }

    auto lines = str.remove(QChar::CarriageReturn).split(QChar::LineFeed);

    m_leftover_line = lines.takeLast();
    return lines;
}

void LoggedProcess::on_stdErr()
{
    auto lines = reprocess(readAllStandardError(), m_err_decoder);
    emit log(lines, MessageLevel::StdErr);
}

void LoggedProcess::on_stdOut()
{
    auto lines = reprocess(readAllStandardOutput(), m_out_decoder);
    emit log(lines, MessageLevel::StdOut);
}

void LoggedProcess::on_exit(int exit_code, QProcess::ExitStatus status)
{
    // save the exit code
    m_exit_code = exit_code;

    // based on state, send signals
    if (!m_is_aborting) {
        if (status == QProcess::NormalExit) {
            //: Message displayed on instance exit
            emit log({ tr("Process exited with code %1.").arg(exit_code) }, MessageLevel::Launcher);
            changeState(LoggedProcess::Finished);
        } else {
            //: Message displayed on instance crashed
            if (exit_code == -1)
                emit log({ tr("Process crashed.") }, MessageLevel::Launcher);
            else
                emit log({ tr("Process crashed with exitcode %1.").arg(exit_code) }, MessageLevel::Launcher);
            changeState(LoggedProcess::Crashed);
        }
    } else {
        //: Message displayed after the instance exits due to kill request
        emit log({ tr("Process was killed by user.") }, MessageLevel::Error);
        changeState(LoggedProcess::Aborted);
    }
}

void LoggedProcess::on_error(QProcess::ProcessError error)
{
    switch (error) {
        case QProcess::FailedToStart: {
            emit log({ tr("The process failed to start.") }, MessageLevel::Fatal);
            changeState(LoggedProcess::FailedToStart);
            break;
        }
        // we'll just ignore those... never needed them
        case QProcess::Crashed:
        case QProcess::ReadError:
        case QProcess::Timedout:
        case QProcess::UnknownError:
        case QProcess::WriteError:
            break;
    }
}

void LoggedProcess::kill()
{
    m_is_aborting = true;
    QProcess::kill();
}

int LoggedProcess::exitCode() const
{
    return m_exit_code;
}

void LoggedProcess::changeState(LoggedProcess::State state)
{
    if (state == m_state)
        return;
    m_state = state;
    emit stateChanged(m_state);
}

LoggedProcess::State LoggedProcess::state() const
{
    return m_state;
}

void LoggedProcess::on_stateChange(QProcess::ProcessState state)
{
    switch (state) {
        case QProcess::NotRunning:
            break;  // let's not - there are too many that handle this already.
        case QProcess::Starting: {
            if (m_state != LoggedProcess::NotRunning) {
                qWarning() << "Wrong state change for process from state" << m_state << "to" << (int)LoggedProcess::Starting;
            }
            changeState(LoggedProcess::Starting);
            return;
        }
        case QProcess::Running: {
            if (m_state != LoggedProcess::Starting) {
                qWarning() << "Wrong state change for process from state" << m_state << "to" << (int)LoggedProcess::Running;
            }
            changeState(LoggedProcess::Running);
            return;
        }
    }
}

void LoggedProcess::setDetachable(bool detachable)
{
    m_is_detachable = detachable;
}
