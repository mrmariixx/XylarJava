#include "net/ApiClient.h"

#include <QNetworkRequest>

namespace xylar {

ApiClient::ApiClient(QObject *parent)
    : QObject(parent)
{
}

QNetworkRequest ApiClient::requestFor(const QUrl &url) const
{
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("XylarJava/0.1"));
    return request;
}

} // namespace xylar
