#pragma once

#include <QDir>
#include <QMap>
#include <QObject>
#include <QRunnable>
#include <memory>
#include "minecraft/mod/Mod.h"
#include "tasks/Task.h"

class ResourceFolderLoadTask : public Task {
    Q_OBJECT
   public:
    struct Result {
        QMap<QString, Resource::Ptr> resources;
    };
    using ResultPtr = std::shared_ptr<Result>;
    ResultPtr result() const { return m_result; }

   public:
    ResourceFolderLoadTask(const QDir& resource_dir,
                           const QDir& index_dir,
                           bool is_indexed,
                           bool clean_orphan,
                           std::function<Resource*(const QFileInfo&)> create_function);

    bool canAbort() const override { return true; }
    bool abort() override
    {
        m_aborted.store(true);
        return true;
    }

    void executeTask() override;

   private:
    void getFromMetadata();

   private:
    QDir m_resource_dir, m_index_dir;
    bool m_is_indexed;
    bool m_clean_orphan;
    std::function<Resource*(QFileInfo const&)> m_create_func;
    ResultPtr m_result;

    std::atomic<bool> m_aborted = false;

    /** This is the thread in which we should put new mod objects */
    QThread* m_thread_to_spawn_into;
};
