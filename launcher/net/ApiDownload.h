#pragma once

#include "Download.h"

namespace Net {

namespace ApiDownload {
Download::Ptr makeCached(QUrl url, MetaEntryPtr entry, Download::Options options = Download::Option::NoOptions);
std::pair<Download::Ptr, QByteArray*> makeByteArray(QUrl url, Download::Options options = Download::Option::NoOptions);
Download::Ptr makeFile(QUrl url, QString path, Download::Options options = Download::Option::NoOptions);
};  // namespace ApiDownload

}  // namespace Net
