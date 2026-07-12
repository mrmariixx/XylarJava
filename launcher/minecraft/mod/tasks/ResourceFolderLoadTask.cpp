#include "ResourceFolderLoadTask.h"

#include "Application.h"
#include "FileSystem.h"
#include "minecraft/mod/MetadataHandler.h"

#include <QThread>

ResourceFolderLoadTask::ResourceFolderLoadTask(const QDir& resource_dir,
                                               const QDir& index_dir,
                                               bool is_indexed,
                                               bool clean_orphan,
                                               std::function<Resource*(const QFileInfo&)> create_function)
    : Task(false)
    , m_resource_dir(resource_dir)
    , m_index_dir(index_dir)
    , m_is_indexed(is_indexed)
    , m_clean_orphan(clean_orphan)
    , m_create_func(create_function)
    , m_result(new Result())
    , m_thread_to_spawn_into(thread())
{}

void ResourceFolderLoadTask::executeTask()
{
    if (thread() != m_thread_to_spawn_into)
        connect(this, &Task::finished, this->thread(), &QThread::quit);

    if (m_is_indexed) {
        // Read metadata first
        getFromMetadata();
    }

    // Read JAR files that don't have metadata
    m_resource_dir.refresh();
    for (auto entry : m_resource_dir.entryInfoList()) {
        auto filePath = entry.absoluteFilePath();
        if (auto app = APPLICATION_DYN; app && app->checkQSavePath(filePath)) {
            continue;
        }
        auto newFilePath = FS::getUniqueResourceName(filePath);
        if (newFilePath != filePath) {
            FS::move(filePath, newFilePath);
            entry = QFileInfo(newFilePath);
        }

        Resource* resource = m_create_func(entry);

        if (resource->enabled()) {
            if (m_result->resources.contains(resource->internal_id())) {
                m_result->resources[resource->internal_id()]->setStatus(ResourceStatus::INSTALLED);
                // Delete the object we just created, since a valid one is already in the mods list.
                delete resource;
            } else {
                m_result->resources[resource->internal_id()].reset(resource);
                m_result->resources[resource->internal_id()]->setStatus(ResourceStatus::NO_METADATA);
            }
        } else {
            QString chopped_id = resource->internal_id().chopped(9);
            if (m_result->resources.contains(chopped_id)) {
                m_result->resources[resource->internal_id()].reset(resource);

                auto metadata = m_result->resources[chopped_id]->metadata();
                if (metadata) {
                    resource->setMetadata(*metadata);

                    m_result->resources[resource->internal_id()]->setStatus(ResourceStatus::INSTALLED);
                    m_result->resources.remove(chopped_id);
                }
            } else {
                m_result->resources[resource->internal_id()].reset(resource);
                m_result->resources[resource->internal_id()]->setStatus(ResourceStatus::NO_METADATA);
            }
        }
    }

    // Remove orphan metadata to prevent issues
    // See https://github.com/PolyMC/PolyMC/issues/996
    if (m_clean_orphan) {
        QMutableMapIterator iter(m_result->resources);
        while (iter.hasNext()) {
            auto resource = iter.next().value();
            if (resource->status() == ResourceStatus::NOT_INSTALLED) {
                resource->destroy(m_index_dir, false, false);
                iter.remove();
            }
        }
    }

    for (auto mod : m_result->resources)
        mod->moveToThread(m_thread_to_spawn_into);

    if (m_aborted)
        emit finished();
    else
        emitSucceeded();
}

void ResourceFolderLoadTask::getFromMetadata()
{
    m_index_dir.refresh();
    for (auto entry : m_index_dir.entryList(QDir::Files)) {
        if (!entry.endsWith(".pw.toml")) {
            continue;
        }

        auto metadata = Metadata::get(m_index_dir, entry);

        if (!metadata.isValid())
            continue;

        auto* resource = m_create_func(QFileInfo(m_resource_dir.filePath(metadata.filename)));
        resource->setMetadata(metadata);
        resource->setStatus(ResourceStatus::NOT_INSTALLED);
        m_result->resources[resource->internal_id()].reset(resource);
    }
}
