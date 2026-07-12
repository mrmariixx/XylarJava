#include "net/ApiDownload.h"
#include "net/ApiHeaderProxy.h"

namespace Net {

Download::Ptr ApiDownload::makeCached(QUrl url, MetaEntryPtr entry, Download::Options options)
{
    auto dl = Download::makeCached(url, entry, options);
    dl->addHeaderProxy(std::make_unique<ApiHeaderProxy>());
    return dl;
}

std::pair<Download::Ptr, QByteArray*> ApiDownload::makeByteArray(QUrl url, Download::Options options)
{
    auto [dl, response] = Download::makeByteArray(url, options);
    dl->addHeaderProxy(std::make_unique<ApiHeaderProxy>());
    return { dl, response };
}

Download::Ptr ApiDownload::makeFile(QUrl url, QString path, Download::Options options)
{
    auto dl = Download::makeFile(url, path, options);
    dl->addHeaderProxy(std::make_unique<ApiHeaderProxy>());
    return dl;
}

}  // namespace Net
