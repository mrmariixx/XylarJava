#include "auth/AuthManager.h"

namespace xylar {

QString AuthManager::currentAccountLabel() const
{
    return hasMicrosoftSession()
        ? QStringLiteral("Microsoft session cached.")
        : QStringLiteral("Offline account active. Microsoft auth can be connected later.");
}

bool AuthManager::hasMicrosoftSession() const
{
    return false;
}

} // namespace xylar
