#include "TexturePack.h"

#include <QDebug>
#include <QMap>
#include "MTPixmapCache.h"

#include "minecraft/mod/tasks/LocalTexturePackParseTask.h"

void TexturePack::setDescription(QString new_description)
{
    QMutexLocker locker(&m_data_lock);

    m_description = new_description;
}

void TexturePack::setImage(QImage new_image) const
{
    QMutexLocker locker(&m_data_lock);

    Q_ASSERT(!new_image.isNull());

    if (m_pack_image_cache_key.key.isValid())
        PixmapCache::remove(m_pack_image_cache_key.key);

    // scale the image to avoid flooding the pixmapcache
    auto pixmap =
        QPixmap::fromImage(new_image.scaled({ 64, 64 }, Qt::AspectRatioMode::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

    m_pack_image_cache_key.key = PixmapCache::insert(pixmap);
    m_pack_image_cache_key.was_ever_used = true;
}

QPixmap TexturePack::image(QSize size, Qt::AspectRatioMode mode) const
{
    QPixmap cached_image;
    if (PixmapCache::find(m_pack_image_cache_key.key, &cached_image)) {
        if (size.isNull())
            return cached_image;
        return cached_image.scaled(size, mode, Qt::SmoothTransformation);
    }

    // No valid image we can get
    if (!m_pack_image_cache_key.was_ever_used) {
        return {};
    } else {
        qDebug() << "Texture Pack" << name() << "Had it's image evicted from the cache. reloading...";
        PixmapCache::markCacheMissByEviciton();
    }

    // Imaged got evicted from the cache. Re-process it and retry.
    TexturePackUtils::processPackPNG(*this);
    return image(size);
}

bool TexturePack::valid() const
{
    return m_description != nullptr;
}
