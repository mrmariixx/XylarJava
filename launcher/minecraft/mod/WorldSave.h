#pragma once

#include "Resource.h"

#include <QMutex>

class Version;

enum class WorldSaveFormat { SINGLE, MULTI, INVALID };

class WorldSave : public Resource {
    Q_OBJECT
   public:
    using Ptr = shared_qobject_ptr<Resource>;

    WorldSave(QObject* parent = nullptr) : Resource(parent) {}
    WorldSave(QFileInfo file_info) : Resource(file_info) {}

    /** Gets the format of the save. */
    WorldSaveFormat saveFormat() const { return m_save_format; }
    /** Gets the name of the save dir (first found in multi mode). */
    QString saveDirName() const { return m_save_dir_name; }

    /** Thread-safe. */
    void setSaveFormat(WorldSaveFormat new_save_format);
    /** Thread-safe. */
    void setSaveDirName(QString dir_name);

    bool valid() const override;

   protected:
    mutable QMutex m_data_lock;

    /** The format in which the save file is in.
     *  Since saves can be distributed in various slightly different ways, this allows us to treat them separately.
     */
    WorldSaveFormat m_save_format = WorldSaveFormat::INVALID;

    QString m_save_dir_name;
};
