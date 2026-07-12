#include "BaseVersionList.h"
#include "BaseVersion.h"

BaseVersionList::BaseVersionList(QObject* parent) : QAbstractListModel(parent) {}

BaseVersion::Ptr BaseVersionList::findVersion(const QString& descriptor)
{
    for (int i = 0; i < count(); i++) {
        if (at(i)->descriptor() == descriptor)
            return at(i);
    }
    return nullptr;
}

BaseVersion::Ptr BaseVersionList::getRecommended() const
{
    if (count() <= 0)
        return nullptr;
    else
        return at(0);
}

QVariant BaseVersionList::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() > count())
        return QVariant();

    BaseVersion::Ptr version = at(index.row());

    switch (role) {
        case VersionPointerRole:
            return QVariant::fromValue(version);

        case VersionRole:
            return version->name();

        case VersionIdRole:
            return version->descriptor();

        case TypeRole:
            return version->typeString();

        case JavaMajorRole: {
            auto major = version->name();
            if (major.startsWith("java")) {
                major = "Java " + major.mid(4);
            }
            return major;
        }

        default:
            return QVariant();
    }
}

BaseVersionList::RoleList BaseVersionList::providesRoles() const
{
    return { VersionPointerRole, VersionRole, VersionIdRole, TypeRole };
}

int BaseVersionList::rowCount(const QModelIndex& parent) const
{
    // Return count
    return parent.isValid() ? 0 : count();
}

int BaseVersionList::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : 1;
}

QHash<int, QByteArray> BaseVersionList::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles.insert(VersionRole, "version");
    roles.insert(VersionIdRole, "versionId");
    roles.insert(ParentVersionRole, "parentGameVersion");
    roles.insert(RecommendedRole, "recommended");
    roles.insert(LatestRole, "latest");
    roles.insert(TypeRole, "type");
    roles.insert(BranchRole, "branch");
    roles.insert(PathRole, "path");
    roles.insert(JavaNameRole, "javaName");
    roles.insert(CPUArchitectureRole, "architecture");
    roles.insert(JavaMajorRole, "javaMajor");
    return roles;
}
