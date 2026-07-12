#pragma once

#include <utility>

#include "HttpMetaCache.h"

#include "QObjectPtr.h"
#include "net/NetRequest.h"

namespace Net {
class ByteArraySink;

class Download : public NetRequest {
    Q_OBJECT
   public:
    using Ptr = shared_qobject_ptr<class Download>;
    explicit Download() : NetRequest() { logCat = taskDownloadLogC; }

#if defined(LAUNCHER_APPLICATION)
    static auto makeCached(QUrl url, MetaEntryPtr entry, Options options = Option::NoOptions) -> Download::Ptr;
#endif

    /**
     * Creates a request downloading to the returned QByteArray,.
     * The QByteArray will live as long as the Download object.
     */
    static auto makeByteArray(QUrl url, Options options = Option::NoOptions) -> std::pair<Download::Ptr, QByteArray*>;
    static auto makeFile(QUrl url, QString path, Options options = Option::NoOptions) -> Download::Ptr;

   protected:
    virtual QNetworkReply* getReply(QNetworkRequest&) override;
};
}  // namespace Net
