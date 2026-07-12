#pragma once

namespace Net {

class DummySink : public Sink {
   public:
    explicit DummySink() {}
    ~DummySink() override {}
    auto init(QNetworkRequest& request) -> Task::State override { return Task::State::Running; }
    auto write(QByteArray& data) -> Task::State override { return Task::State::Succeeded; }
    auto abort() -> Task::State override { return Task::State::AbortedByUser; }
    auto finalize(QNetworkReply& reply) -> Task::State override { return Task::State::Succeeded; }
    auto hasLocalData() -> bool override { return false; }
};

}  // namespace Net
