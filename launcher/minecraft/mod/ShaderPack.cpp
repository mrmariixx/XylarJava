#include "ShaderPack.h"

void ShaderPack::setPackFormat(ShaderPackFormat new_format)
{
    QMutexLocker locker(&m_data_lock);

    m_pack_format = new_format;
}

bool ShaderPack::valid() const
{
    return m_pack_format != ShaderPackFormat::INVALID;
}
