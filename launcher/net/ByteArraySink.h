#pragma once

#include "Sink.h"

namespace Net {

/*
 * Sink object for downloads that uses an owned QByteArray as a target.
 */
class ByteArraySink : public Sink {
   public:
    virtual ~ByteArraySink() = default;

   public:
    auto init(QNetworkRequest& request) -> Task::State override
    {
        m_output.clear();
        if (initAllValidators(request))
            return Task::State::Running;
        m_fail_reason = "Failed to initialize validators";
        return Task::State::Failed;
    };

    auto write(QByteArray& data) -> Task::State override
    {
        m_output.append(data);
        if (writeAllValidators(data))
            return Task::State::Running;
        m_fail_reason = "Failed to write validators";
        return Task::State::Failed;
    }

    auto abort() -> Task::State override
    {
        failAllValidators();
        m_fail_reason = "Aborted";
        return Task::State::Failed;
    }

    auto finalize(QNetworkReply& reply) -> Task::State override
    {
        if (finalizeAllValidators(reply))
            return Task::State::Succeeded;
        m_fail_reason = "Failed to finalize validators";
        return Task::State::Failed;
    }

    auto hasLocalData() -> bool override { return false; }

    QByteArray* output() { return &m_output; }

   protected:
    QByteArray m_output;
};
}  // namespace Net
