#pragma once

#include <QByteArray>
#include <QString>

namespace xylar {

class Hash
{
public:
    [[nodiscard]] static QString sha1Hex(const QByteArray &data);
    [[nodiscard]] static QString sha256Hex(const QByteArray &data);
};

} // namespace xylar
