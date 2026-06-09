#pragma once

#include <QString>

namespace xylar {

class Logger
{
public:
    static void installMessageHandler();
    static void info(const QString &message);
    static void warning(const QString &message);
};

} // namespace xylar
