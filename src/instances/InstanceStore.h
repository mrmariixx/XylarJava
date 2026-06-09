#pragma once

#include <QList>
#include <QString>

#include "instances/Instance.h"

namespace xylar {

class InstanceStore
{
public:
    [[nodiscard]] QList<Instance> loadInstances() const;
    [[nodiscard]] Instance loadInstance(const QString &id) const;
    [[nodiscard]] bool saveInstance(const Instance &instance, QString *errorMessage = nullptr) const;
    [[nodiscard]] Instance ensureInstance(const QString &versionId, const QString &name, QString *errorMessage = nullptr) const;

private:
    [[nodiscard]] QString instanceRoot(const QString &id) const;
    [[nodiscard]] QString instanceJsonPath(const QString &id) const;
};

} // namespace xylar
