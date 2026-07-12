#include <QJsonArray>
#include <QJsonObject>

#include "Rule.h"

Rule Rule::fromJson(const QJsonObject& object)
{
    Rule result;

    if (object["action"] == "allow")
        result.m_action = Allow;
    else if (object["action"] == "disallow")
        result.m_action = Disallow;

    if (auto os = object["os"]; os.isObject()) {
        if (auto name = os["name"].toString(); !name.isNull()) {
            result.m_os = OS{
                name,
                os["version"].toString(),
            };
        }
    }

    return result;
}

QJsonObject Rule::toJson()
{
    QJsonObject result;

    if (m_action == Allow)
        result["action"] = "allow";
    else if (m_action == Disallow)
        result["action"] = "disallow";

    if (m_os.has_value()) {
        QJsonObject os;

        os["name"] = m_os->name;

        if (!m_os->version.isEmpty())
            os["version"] = m_os->version;

        result["os"] = os;
    }

    return result;
}

Rule::Action Rule::apply(const RuntimeContext& runtimeContext)
{
    if (m_os.has_value() && !runtimeContext.classifierMatches(m_os->name))
        return Defer;

    return m_action;
}
