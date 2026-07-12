#pragma once

#include <QAbstractListModel>
#include <QObject>

#include "BaseVersionList.h"
#include "java/JavaChecker.h"
#include "tasks/Task.h"

#include "JavaInstall.h"

#include "QObjectPtr.h"

class JavaListLoadTask;

class JavaInstallList : public BaseVersionList {
    Q_OBJECT
    enum class Status { NotDone, InProgress, Done };

   public:
    explicit JavaInstallList(QObject* parent = 0, bool onlyManagedVersions = false);

    Task::Ptr getLoadTask() override;
    bool isLoaded() override;
    const BaseVersion::Ptr at(int i) const override;
    int count() const override;
    void sortVersions() override;

    QVariant data(const QModelIndex& index, int role) const override;
    RoleList providesRoles() const override;

   public slots:
    void updateListData(QList<BaseVersion::Ptr> versions) override;

   protected:
    void load();
    Task::Ptr getCurrentTask();

   protected:
    Status m_status = Status::NotDone;
    shared_qobject_ptr<JavaListLoadTask> m_load_task;
    QList<BaseVersion::Ptr> m_vlist;
    bool m_only_managed_versions;
};

class JavaListLoadTask : public Task {
    Q_OBJECT

   public:
    explicit JavaListLoadTask(JavaInstallList* vlist, bool onlyManagedVersions = false);
    virtual ~JavaListLoadTask() = default;

   protected:
    void executeTask() override;
   public slots:
    void javaCheckerFinished();

   protected:
    Task::Ptr m_job;
    JavaInstallList* m_list;
    JavaInstall* m_current_recommended;
    QList<JavaChecker::Result> m_results;
    bool m_only_managed_versions;
};
