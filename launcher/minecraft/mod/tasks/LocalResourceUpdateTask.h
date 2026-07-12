#pragma once

#include <QDir>

#include "modplatform/ModIndex.h"
#include "tasks/Task.h"

class LocalResourceUpdateTask : public Task {
    Q_OBJECT
   public:
    using Ptr = shared_qobject_ptr<LocalResourceUpdateTask>;

    explicit LocalResourceUpdateTask(QDir index_dir, ModPlatform::IndexedPack& project, ModPlatform::IndexedVersion& version);

    auto canAbort() const -> bool override { return true; }
    auto abort() -> bool override;

   protected slots:
    //! Entry point for tasks.
    void executeTask() override;

   signals:
    void hasOldResource(QString name, QString filename);

   private:
    QDir m_index_dir;
    ModPlatform::IndexedPack m_project;
    ModPlatform::IndexedVersion m_version;
};
