#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QObject>
#include <QPair>
#include <QSet>
#include <QStack>

#include "BaseInstance.h"

class QFileSystemWatcher;
class InstanceTask;
struct InstanceName;

using InstanceId = QString;
using GroupId = QString;
using InstanceLocator = std::pair<BaseInstance*, int>;

enum class InstCreateError { NoCreateError = 0, NoSuchVersion, UnknownCreateError, InstExists, CantCreateDir };

enum class GroupsState { NotLoaded, Steady, Dirty };

struct TrashShortcutItem {
    ShortcutData data;
    QString trashPath;
};

struct TrashHistoryItem {
    QString id;
    QString path;
    QString trashPath;
    QString groupName;
    QList<TrashShortcutItem> shortcuts;
};

class InstanceList : public QAbstractListModel {
    Q_OBJECT

   public:
    explicit InstanceList(SettingsObject* settings, const QString& instDir, QObject* parent = 0);
    virtual ~InstanceList();

   public:
    QModelIndex index(int row, int column = 0, const QModelIndex& parent = QModelIndex()) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    enum AdditionalRoles {
        GroupRole = Qt::UserRole,
        InstancePointerRole = 0x34B1CB48,  ///< Return pointer to real instance
        InstanceIDRole = 0x34B1CB49        ///< Return id if the instance
    };
    /*!
     * \brief Error codes returned by functions in the InstanceList class.
     * NoError Indicates that no error occurred.
     * UnknownError indicates that an unspecified error occurred.
     */
    enum InstListError { NoError = 0, UnknownError };

    BaseInstance* at(int i) const { return m_instances.at(i).get(); }

    int count() const { return static_cast<int>(m_instances.size()); }

    InstListError loadList();
    void saveNow();

    /* O(n) */
    BaseInstance* getInstanceById(QString id) const;
    /* O(n) */
    BaseInstance* getInstanceByManagedName(const QString& managed_name) const;
    QModelIndex getInstanceIndexById(const QString& id) const;
    QStringList getGroups();
    bool isGroupCollapsed(const QString& groupName);

    GroupId getInstanceGroup(const InstanceId& id) const;
    void setInstanceGroup(const InstanceId& id, GroupId name);

    void deleteGroup(const GroupId& name);
    void renameGroup(const GroupId& src, const GroupId& dst);
    bool trashInstance(const InstanceId& id);
    bool trashedSomething() const;
    bool undoTrashInstance();
    void deleteInstance(const InstanceId& id);

    // Wrap an instance creation task in some more task machinery and make it ready to be used
    Task* wrapInstanceTask(InstanceTask* task);

    /**
     * Create a new empty staging area for instance creation and @return a path/key top commit it later.
     * Used by instance manipulation tasks.
     */
    QString getStagedInstancePath();

    /**
     * Commit the staging area given by @keyPath to the provider - used when creation succeeds.
     * Used by instance manipulation tasks.
     * should_override is used when another similar instance already exists, and we want to override it
     * - for instance, when updating it.
     */
    bool commitStagedInstance(const QString& keyPath, const InstanceName& instanceName, QString groupName, const InstanceTask&);

    /**
     * Destroy a previously created staging area given by @keyPath - used when creation fails.
     * Used by instance manipulation tasks.
     */
    bool destroyStagingPath(const QString& keyPath);

    int getTotalPlayTime();

    Qt::DropActions supportedDragActions() const override;

    Qt::DropActions supportedDropActions() const override;

    bool canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const override;

    bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override;

    QStringList mimeTypes() const override;
    QMimeData* mimeData(const QModelIndexList& indexes) const override;

    QStringList getLinkedInstancesById(const QString& id) const;

   signals:
    void dataIsInvalid();
    void instancesChanged();
    void instanceSelectRequest(QString instanceId);
    void groupsChanged(QSet<QString> groups);

   public slots:
    void on_InstFolderChanged(const Setting& setting, QVariant value);
    void on_GroupStateChanged(const QString& group, bool collapsed);

   private slots:
    void propertiesChanged(BaseInstance* inst);
    void providerUpdated();
    void instanceDirContentsChanged(const QString& path);

   private:
    int getInstIndex(BaseInstance* inst) const;
    void updateTotalPlayTime();
    void suspendWatch();
    void resumeWatch();
    void add(std::vector<std::unique_ptr<BaseInstance>>& list);
    void loadGroupList();
    void saveGroupList();
    QList<InstanceId> discoverInstances();
    std::unique_ptr<BaseInstance> loadInstance(const InstanceId& id);

    void increaseGroupCount(const QString& group);
    void decreaseGroupCount(const QString& group);

   private:
    int m_watchLevel = 0;
    int totalPlayTime = 0;
    bool m_dirty = false;
    std::vector<std::unique_ptr<BaseInstance>> m_instances;
    // id -> refs
    QMap<QString, int> m_groupNameCache;

    SettingsObject* m_globalSettings;
    QString m_instDir;
    QFileSystemWatcher* m_watcher;
    // FIXME: this is so inefficient that looking at it is almost painful.
    QSet<QString> m_collapsedGroups;
    QMap<InstanceId, GroupId> m_instanceGroupIndex;
    QSet<InstanceId> instanceSet;
    bool m_groupsLoaded = false;
    bool m_instancesProbed = false;

    QStack<TrashHistoryItem> m_trashHistory;
};
