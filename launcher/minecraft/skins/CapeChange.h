#pragma once

#include "net/NetRequest.h"

class CapeChange : public Net::NetRequest {
    Q_OBJECT
   public:
    using Ptr = shared_qobject_ptr<CapeChange>;
    CapeChange(QString capeId);
    virtual ~CapeChange() = default;

    static CapeChange::Ptr make(QString token, QString capeId);

   protected:
    virtual QNetworkReply* getReply(QNetworkRequest&) override;

   private:
    QString m_capeId;
};
