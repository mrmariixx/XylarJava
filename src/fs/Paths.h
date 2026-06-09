#pragma once

#include <QDir>
#include <QString>

namespace xylar {

class Paths
{
public:
    [[nodiscard]] static QDir appDataDir();
    [[nodiscard]] static QDir metadataDir();
    [[nodiscard]] static QDir versionsDir();
    [[nodiscard]] static QDir instancesDir();
    [[nodiscard]] static QDir librariesDir();
    [[nodiscard]] static QDir assetsDir();
    [[nodiscard]] static QDir runtimesDir();
};

} // namespace xylar
