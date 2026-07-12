#pragma once

#include <QJsonObject>
#include <QList>
#include <QString>
#include "RuntimeContext.h"

class Library;

class Rule {
   public:
    enum Action { Allow, Disallow, Defer };

    static Rule fromJson(const QJsonObject& json);
    QJsonObject toJson();

    Action apply(const RuntimeContext& runtimeContext);

   private:
    struct OS {
        QString name;
        // FIXME: unsupported
        // retained to avoid information being lost from files
        QString version;
    };

    Action m_action = Defer;
    std::optional<OS> m_os;
};
