#pragma once

#include "Resource.h"

/* Info:
 * Currently For Optifine / Iris shader packs,
 * could be expanded to support others should they exist?
 *
 * This class and enum are mostly here as placeholders for validating
 * that a shaderpack exists and is in the right format,
 * namely that they contain a folder named 'shaders'.
 *
 * In the technical sense it would be possible to parse files like `shaders/shaders.properties`
 * to get information like the available profiles but this is not all that useful without more knowledge of the
 * shader mod used to be able to change settings.
 */

#include <QMutex>

enum class ShaderPackFormat { VALID, INVALID };

class ShaderPack : public Resource {
    Q_OBJECT
   public:
    using Ptr = shared_qobject_ptr<Resource>;

    ShaderPackFormat packFormat() const { return m_pack_format; }

    ShaderPack(QObject* parent = nullptr) : Resource(parent) {}
    ShaderPack(QFileInfo file_info) : Resource(file_info) {}

    /** Thread-safe. */
    void setPackFormat(ShaderPackFormat new_format);

    bool valid() const override;

   protected:
    mutable QMutex m_data_lock;

    ShaderPackFormat m_pack_format = ShaderPackFormat::INVALID;
};
