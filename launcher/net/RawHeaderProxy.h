#pragma once

#include "net/HeaderProxy.h"

namespace Net {

class RawHeaderProxy : public HeaderProxy {
   public:
    RawHeaderProxy(QList<HeaderPair> headers = {}) : HeaderProxy(), m_headers(std::move(headers)) {};
    virtual ~RawHeaderProxy() = default;

   public:
    virtual QList<HeaderPair> headers(const QNetworkRequest&) const override { return m_headers; };

    void addHeader(const HeaderPair& header) { m_headers.append(header); }
    void addHeader(const QByteArray& headerName, const QByteArray& headerValue) { m_headers.append({ headerName, headerValue }); }
    void addHeaders(const QList<HeaderPair>& headers) { m_headers.append(headers); }
    void setHeaders(QList<HeaderPair> headers) { m_headers = headers; };

   private:
    QList<HeaderPair> m_headers;
};

}  // namespace Net
