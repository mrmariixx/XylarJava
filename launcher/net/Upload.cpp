#include "Upload.h"

#include <memory>
#include <utility>
#include "ByteArraySink.h"

namespace Net {

QNetworkReply* Upload::getReply(QNetworkRequest& request)
{
    if (!request.hasRawHeader("Content-Type"))
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    return m_network->post(request, m_post_data);
}

std::pair<Upload::Ptr, QByteArray*> Upload::makeByteArray(QUrl url, QByteArray m_post_data)
{
    auto up = makeShared<Upload>();
    up->m_url = std::move(url);

    auto sink = std::make_unique<ByteArraySink>();
    QByteArray* response = sink->output();
    up->m_sink = std::move(sink);

    up->m_post_data = std::move(m_post_data);
    return { up, response };
}
}  // namespace Net
