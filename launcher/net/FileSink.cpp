#include "FileSink.h"

#include "FileSystem.h"

#include "net/Logging.h"

namespace Net {

Task::State FileSink::init(QNetworkRequest& request)
{
    auto result = initCache(request);
    if (result != Task::State::Running) {
        return result;
    }

    // create a new save file and open it for writing
    if (!FS::ensureFilePathExists(m_filename)) {
        qCCritical(taskNetLogC) << "Could not create folder for " + m_filename;
        m_fail_reason = "Could not create folder";
        return Task::State::Failed;
    }

    m_wroteAnyData = false;
    m_output_file.reset(new PSaveFile(m_filename));
    if (!m_output_file->open(QIODevice::WriteOnly)) {
        const auto error = QString("Could not open %1 for writing: %2").arg(m_filename).arg(m_output_file->errorString());
        qCCritical(taskNetLogC) << error;
        m_fail_reason = error;
        return Task::State::Failed;
    }

    if (initAllValidators(request))
        return Task::State::Running;
    m_fail_reason = "Failed to initialize validators";
    return Task::State::Failed;
}

Task::State FileSink::write(QByteArray& data)
{
    if (!writeAllValidators(data) || m_output_file->write(data) != data.size()) {
        QString error = QString("Failed writing into %1: %2").arg(m_filename);
        if (m_output_file->error() == QFileDevice::NoError) {
            error = error.arg("Validators failed");
        } else {
            error = error.arg(m_output_file->errorString());
        }
        qCCritical(taskNetLogC) << error;
        m_fail_reason = error;
        m_output_file->cancelWriting();
        m_output_file.reset();
        m_wroteAnyData = false;
        return Task::State::Failed;
    }

    m_wroteAnyData = true;
    return Task::State::Running;
}

Task::State FileSink::abort()
{
    if (m_output_file) {
        m_output_file->cancelWriting();
    }
    failAllValidators();
    return Task::State::Failed;
}

Task::State FileSink::finalize(QNetworkReply& reply)
{
    bool gotFile = false;
    QVariant statusCodeV = reply.attribute(QNetworkRequest::HttpStatusCodeAttribute);
    bool validStatus = false;
    int statusCode = statusCodeV.toInt(&validStatus);
    if (validStatus) {
        // this leaves out 304 Not Modified
        gotFile = statusCode == 200 || statusCode == 203;
    }

    // if we wrote any data to the save file, we try to commit the data to the real file.
    // if it actually got a proper file, we write it even if it was empty
    if (gotFile || m_wroteAnyData) {
        // ask validators for data consistency
        // we only do this for actual downloads, not 'your data is still the same' cache hits
        if (!finalizeAllValidators(reply)) {
            m_fail_reason = "Failed to finalize validators";
            return Task::State::Failed;
        }

        // nothing went wrong...
        if (!m_output_file->commit()) {
            const auto error = QString("Failed to commit changes to %1: %2").arg(m_filename).arg(m_output_file->errorString());
            qCCritical(taskNetLogC) << error;
            m_fail_reason = error;
            m_output_file->cancelWriting();
            return Task::State::Failed;
        }
    }

    // then get rid of the save file
    m_output_file.reset();

    return finalizeCache(reply);
}

Task::State FileSink::initCache(QNetworkRequest&)
{
    return Task::State::Running;
}

Task::State FileSink::finalizeCache(QNetworkReply&)
{
    return Task::State::Succeeded;
}

bool FileSink::hasLocalData()
{
    QFileInfo info(m_filename);
    return info.exists() && info.size() != 0;
}
}  // namespace Net
