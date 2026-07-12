#pragma once

#include <QtNetwork>

#include <QObject>
#include "net/NetRequest.h"
#include "tasks/ConcurrentTask.h"

// Those are included so that they are also included by anyone using NetJob
#include "net/Download.h"
#include "net/HttpMetaCache.h"

class NetJob : public ConcurrentTask {
    Q_OBJECT

   public:
    // TODO: delete
    using Ptr = shared_qobject_ptr<NetJob>;

    explicit NetJob(QString job_name, QNetworkAccessManager* network, int max_concurrent = -1);
    ~NetJob() override = default;

    auto size() const -> int;

    auto canAbort() const -> bool override;
    auto addNetAction(Net::NetRequest::Ptr action) -> bool;

    auto getFailedActions() -> QList<Net::NetRequest*>;
    auto getFailedFiles() -> QList<QString>;
    void setAskRetry(bool askRetry);

   public slots:
    // Qt can't handle auto at the start for some reason?
    bool abort() override;
    void emitFailed(QString reason) override;

   protected slots:
    void executeNextSubTask() override;

   protected:
    void updateState() override;
    bool isOnline();

   private:
    QNetworkAccessManager* m_network;

    int m_try = 1;
    bool m_ask_retry = true;
    int m_manual_try = 0;
};
