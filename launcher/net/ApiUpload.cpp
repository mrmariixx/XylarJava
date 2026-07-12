#include "net/ApiUpload.h"
#include "net/ApiHeaderProxy.h"

namespace Net {

std::pair<Upload::Ptr, QByteArray*> ApiUpload::makeByteArray(QUrl url, QByteArray m_post_data)
{
    auto [up, response] = Upload::makeByteArray(url, m_post_data);
    up->addHeaderProxy(std::make_unique<ApiHeaderProxy>());
    return { up, response };
}

}  // namespace Net
