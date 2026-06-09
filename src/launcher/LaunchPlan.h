#pragma once

#include <QString>
#include <QStringList>

namespace xylar {

struct LaunchPlan
{
    QString program;
    QStringList arguments;
    QString workingDirectory;
    QString instanceId;
    QString versionId;

    [[nodiscard]] bool isValid() const;
    [[nodiscard]] QString displayCommand() const;
};

} // namespace xylar
