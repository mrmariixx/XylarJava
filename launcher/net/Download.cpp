#include "Download.h"
#include <QUrl>

#include <QDateTime>
#include <QFileInfo>
#include <memory>

#include "ByteArraySink.h"
#include "ChecksumValidator.h"
#include "MetaCacheSink.h"

namespace Net {

#if defined(LAUNCHER_APPLICATION)
auto Download::makeCached(QUrl url, MetaEntryPtr entry, Options options) -> Download::Ptr
{
    auto dl = makeShared<Download>();
    dl->m_url = url;
    dl->setObjectName(QString("CACHE:") + url.toString());
    dl->m_options = options;
    auto md5Node = new ChecksumValidator(QCryptographicHash::Md5);
    auto cachedNode = new MetaCacheSink(entry, md5Node, options.testFlag(Option::MakeEternal));
    dl->m_sink.reset(cachedNode);
    return dl;
}
#endif

auto Download::makeByteArray(QUrl url, Options options) -> std::pair<Download::Ptr, QByteArray*>
{
    auto dl = makeShared<Download>();
    dl->m_url = url;
    dl->setObjectName(QString("BYTES:") + url.toString());
    dl->m_options = options;

    auto sink = std::make_unique<ByteArraySink>();
    QByteArray* response = sink->output();
    dl->m_sink = std::move(sink);

    return { dl, response };
}

auto Download::makeFile(QUrl url, QString path, Options options) -> Download::Ptr
{
    auto dl = makeShared<Download>();
    dl->m_url = url;
    dl->setObjectName(QString("FILE:") + url.toString());
    dl->m_options = options;
    dl->m_sink.reset(new FileSink(path));
    return dl;
}

QNetworkReply* Download::getReply(QNetworkRequest& request)
{
    return m_network->get(request);
}
}  // namespace Net
