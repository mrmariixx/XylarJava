#pragma once

#include <QString>

namespace xylar {

class AuthManager
{
public:
    [[nodiscard]] QString currentAccountLabel() const;
    [[nodiscard]] bool hasMicrosoftSession() const;
};

} // namespace xylar
