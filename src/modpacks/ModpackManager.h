#pragma once

#include <QStringList>

namespace xylar {

class ModpackManager
{
public:
    [[nodiscard]] QStringList supportedSources() const;
};

} // namespace xylar
