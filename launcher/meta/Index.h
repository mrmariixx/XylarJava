#pragma once

#include <QAbstractListModel>

#include "BaseEntity.h"
#include "meta/VersionList.h"
#include "net/Mode.h"

class Task;

namespace Meta {

class Index : public QAbstractListModel, public BaseEntity {
    Q_OBJECT
   public:
    explicit Index(QObject* parent = nullptr);
    explicit Index(const QList<VersionList::Ptr>& lists, QObject* parent = nullptr);
    virtual ~Index() = default;

    enum { UidRole = Qt::UserRole, NameRole, ListPtrRole };

    QVariant data(const QModelIndex& index, int role) const override;
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QString localFilename() const override { return "index.json"; }

    // queries
    VersionList::Ptr get(const QString& uid);
    Version::Ptr get(const QString& uid, const QString& version);
    bool hasUid(const QString& uid) const;

    QList<VersionList::Ptr> lists() const { return m_lists; }

    Task::Ptr loadVersion(const QString& uid, const QString& version = {}, Net::Mode mode = Net::Mode::Online, bool force = false);

    // this blocks until the version is loaded
    Version::Ptr getLoadedVersion(const QString& uid, const QString& version);

   public:  // for usage by parsers only
    void merge(const std::shared_ptr<Index>& other);

   protected:
    void parse(const QJsonObject& obj) override;

   private:
    QList<VersionList::Ptr> m_lists;
    QHash<QString, VersionList::Ptr> m_uids;

    void connectVersionList(int row, const VersionList::Ptr& list);
};
}  // namespace Meta
