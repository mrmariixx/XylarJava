#include "WorldSave.h"

#include "minecraft/mod/tasks/LocalWorldSaveParseTask.h"

void WorldSave::setSaveFormat(WorldSaveFormat new_save_format)
{
    QMutexLocker locker(&m_data_lock);

    m_save_format = new_save_format;
}

void WorldSave::setSaveDirName(QString dir_name)
{
    QMutexLocker locker(&m_data_lock);

    m_save_dir_name = dir_name;
}

bool WorldSave::valid() const
{
    return m_save_format != WorldSaveFormat::INVALID;
}
