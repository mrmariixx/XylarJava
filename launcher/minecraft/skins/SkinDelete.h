#pragma once

#include "net/NetRequest.h"

class SkinDelete : public Net::NetRequest {
    Q_OBJECT
   public:
    using Ptr = shared_qobject_ptr<SkinDelete>;
    SkinDelete();
    virtual ~SkinDelete() = default;

    static SkinDelete::Ptr make(QString token);

   protected:
    virtual QNetworkReply* getReply(QNetworkRequest&) override;
};
