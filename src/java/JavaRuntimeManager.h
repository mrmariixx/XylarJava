#pragma once

#include <QString>

namespace xylar {

class JavaRuntimeManager
{
public:
    [[nodiscard]] QString preferredRuntimeName() const;
    [[nodiscard]] QString preferredJavaExecutable() const;
};

} // namespace xylar
