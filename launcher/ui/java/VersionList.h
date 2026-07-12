#pragma once

#include "BaseVersionList.h"
#include "java/JavaMetadata.h"
#include "meta/Version.h"

namespace Java {

class VersionList : public BaseVersionList {
    Q_OBJECT

   public:
    explicit VersionList(Meta::Version::Ptr m_version, QObject* parent = 0);

    Task::Ptr getLoadTask() override;
    bool isLoaded() override;
    const BaseVersion::Ptr at(int i) const override;
    int count() const override;
    void sortVersions() override;

    QVariant data(const QModelIndex& index, int role) const override;
    RoleList providesRoles() const override;

   protected slots:
    void updateListData(QList<BaseVersion::Ptr>) override {}

   protected:
    Meta::Version::Ptr m_version;
    QList<Java::MetadataPtr> m_vlist;
};

}  // namespace Java
