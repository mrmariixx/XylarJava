#pragma once

#include "net/NetRequest.h"

class SkinUpload : public Net::NetRequest {
    Q_OBJECT
   public:
    using Ptr = shared_qobject_ptr<SkinUpload>;

    // Note this class takes ownership of the file.
    SkinUpload(QString path, QString variant);
    virtual ~SkinUpload() = default;

    static SkinUpload::Ptr make(QString token, QString path, QString variant);

   protected:
    virtual QNetworkReply* getReply(QNetworkRequest&) override;

   private:
    QString m_path;
    QString m_variant;
};
