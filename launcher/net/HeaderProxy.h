#pragma once

#include <QDebug>
#include <QNetworkRequest>

namespace Net {

struct HeaderPair {
    QByteArray headerName;
    QByteArray headerValue;
};

class HeaderProxy {
   public:
    HeaderProxy() {}
    virtual ~HeaderProxy() {}

   public:
    virtual QList<HeaderPair> headers(const QNetworkRequest& request) const = 0;

   public:
    void writeHeaders(QNetworkRequest& request)
    {
        for (auto header : headers(request)) {
            request.setRawHeader(header.headerName, header.headerValue);
        }
    }
};

}  // namespace Net
