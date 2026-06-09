#include "launcher/LaunchPlan.h"

#include <QRegularExpression>

namespace {

QString quoteArgument(QString value)
{
    if (!value.contains(QRegularExpression(QStringLiteral("\\s|\"")))) {
        return value;
    }
    value.replace(QStringLiteral("\""), QStringLiteral("\\\""));
    return QStringLiteral("\"%1\"").arg(value);
}

} // namespace

namespace xylar {

bool LaunchPlan::isValid() const
{
    return !program.isEmpty() && !arguments.isEmpty() && !workingDirectory.isEmpty();
}

QString LaunchPlan::displayCommand() const
{
    QStringList command;
    command << quoteArgument(program);
    for (const QString &argument : arguments) {
        command << quoteArgument(argument);
    }
    return command.join(QStringLiteral(" "));
}

} // namespace xylar
