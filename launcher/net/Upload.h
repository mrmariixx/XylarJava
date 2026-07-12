#pragma once

#include <utility>

#include "net/NetRequest.h"

namespace Net {

class Upload : public NetRequest {
    Q_OBJECT
   public:
    using Ptr = shared_qobject_ptr<Upload>;
    explicit Upload() : NetRequest() { logCat = taskUploadLogC; };

    /**
     * Creates a request downloading to the returned QByteArray,.
     * The QByteArray will live as long as the Upload object.
     */
    static std::pair<Upload::Ptr, QByteArray*> makeByteArray(QUrl url, QByteArray m_post_data);

   protected:
    virtual QNetworkReply* getReply(QNetworkRequest&) override;
    QByteArray m_post_data;
};

}  // namespace Net
