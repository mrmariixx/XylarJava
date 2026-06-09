#include "instances/InstanceStore.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>

#include "fs/Paths.h"

namespace xylar {
namespace {

QString safeId(QString value)
{
    value = value.trimmed().toLower();
    value.replace(QRegularExpression(QStringLiteral("[^a-z0-9._-]+")), QStringLiteral("-"));
    value.replace(QRegularExpression(QStringLiteral("-+")), QStringLiteral("-"));
    value = value.trimmed();
    if (value.isEmpty()) {
        value = QStringLiteral("instance");
    }
    return value;
}

Instance fromJson(const QJsonObject &root, const QString &fallbackId, const QString &fallbackRoot)
{
    Instance instance;
    instance.id = root.value(QStringLiteral("id")).toString(fallbackId);
    instance.name = root.value(QStringLiteral("name")).toString(instance.id);
    instance.loader = root.value(QStringLiteral("loader")).toString(QStringLiteral("Vanilla"));
    instance.rootPath = root.value(QStringLiteral("rootPath")).toString(fallbackRoot);
    instance.gamePath = root.value(QStringLiteral("gamePath")).toString(QDir(instance.rootPath).filePath(QStringLiteral(".minecraft")));
    instance.lastLaunch = static_cast<qint64>(root.value(QStringLiteral("lastLaunch")).toDouble(0));
    instance.minecraftVersion.id = root.value(QStringLiteral("minecraftVersion")).toString(QStringLiteral("latest-release"));
    instance.minecraftVersion.type = root.value(QStringLiteral("versionType")).toString(QStringLiteral("release"));
    return instance;
}

QJsonObject toJson(const Instance &instance)
{
    QJsonObject root;
    root.insert(QStringLiteral("id"), instance.id);
    root.insert(QStringLiteral("name"), instance.name);
    root.insert(QStringLiteral("loader"), instance.loader);
    root.insert(QStringLiteral("rootPath"), instance.rootPath);
    root.insert(QStringLiteral("gamePath"), instance.gamePath);
    root.insert(QStringLiteral("lastLaunch"), static_cast<double>(instance.lastLaunch));
    root.insert(QStringLiteral("minecraftVersion"), instance.minecraftVersion.id);
    root.insert(QStringLiteral("versionType"), instance.minecraftVersion.type);
    return root;
}

} // namespace

QList<Instance> InstanceStore::loadInstances() const
{
    QList<Instance> instances;
    const QDir root = Paths::instancesDir();
    const QFileInfoList dirs = root.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    for (const QFileInfo &dir : dirs) {
        const Instance instance = loadInstance(dir.fileName());
        if (instance.isValid()) {
            instances.append(instance);
        }
    }
    return instances;
}

Instance InstanceStore::loadInstance(const QString &id) const
{
    const QString path = instanceJsonPath(id);
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return {};
    }

    const QJsonDocument document = QJsonDocument::fromJson(file.readAll());
    if (!document.isObject()) {
        return {};
    }
    return fromJson(document.object(), id, instanceRoot(id));
}

bool InstanceStore::saveInstance(const Instance &instance, QString *errorMessage) const
{
    if (!instance.isValid()) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Invalid instance.");
        }
        return false;
    }

    QDir().mkpath(instance.rootPath);
    QDir().mkpath(instance.gamePath);
    QDir().mkpath(QDir(instance.gamePath).filePath(QStringLiteral("mods")));
    QDir().mkpath(QDir(instance.rootPath).filePath(QStringLiteral("natives")));

    QFile file(instanceJsonPath(instance.id));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Cannot write %1").arg(file.fileName());
        }
        return false;
    }
    file.write(QJsonDocument(toJson(instance)).toJson(QJsonDocument::Indented));
    return true;
}

Instance InstanceStore::ensureInstance(const QString &versionId, const QString &name, QString *errorMessage) const
{
    const QString id = safeId(name.isEmpty() ? versionId : name);
    Instance existing = loadInstance(id);
    if (existing.isValid()) {
        return existing;
    }

    Instance instance;
    instance.id = id;
    instance.name = name.isEmpty() ? QStringLiteral("Minecraft %1").arg(versionId) : name;
    instance.loader = QStringLiteral("Vanilla");
    instance.rootPath = instanceRoot(id);
    instance.gamePath = QDir(instance.rootPath).filePath(QStringLiteral(".minecraft"));
    instance.lastLaunch = QDateTime::currentMSecsSinceEpoch();
    instance.minecraftVersion.id = versionId;
    instance.minecraftVersion.type = QStringLiteral("release");

    if (!saveInstance(instance, errorMessage)) {
        return {};
    }
    return instance;
}

QString InstanceStore::instanceRoot(const QString &id) const
{
    return Paths::instancesDir().filePath(id);
}

QString InstanceStore::instanceJsonPath(const QString &id) const
{
    return QDir(instanceRoot(id)).filePath(QStringLiteral("instance.json"));
}

} // namespace xylar
