#pragma once

#include <QList>
#include <QString>

namespace xylar {

struct MinecraftVersion
{
    QString id;
    QString type;
    QString url;
    QString releaseTime;

    [[nodiscard]] static MinecraftVersion latestReleasePlaceholder();
};

struct MinecraftInstallResult
{
    bool ok = false;
    QString message;
    int plannedDownloads = 0;
};

} // namespace xylar
