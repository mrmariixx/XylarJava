#pragma once

#include "Upload.h"

namespace Net {

namespace ApiUpload {
std::pair<Upload::Ptr, QByteArray*> makeByteArray(QUrl url, QByteArray m_post_data);
};

}  // namespace Net
