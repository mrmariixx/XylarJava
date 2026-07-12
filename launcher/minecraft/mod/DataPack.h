#pragma once

#include "Resource.h"

#include <QMutex>
#include <QPixmapCache>
#include <utility>

class Version;

/* TODO:
 *
 * Store localized descriptions
 * */

class DataPack : public Resource {
    Q_OBJECT
   public:
    DataPack(QObject* parent = nullptr) : Resource(parent) {}
    DataPack(QFileInfo file_info) : Resource(file_info) {}

    /** Gets the numerical ID of the pack format. */
    int packFormat() const { return m_pack_format; }

    /** Gets the description of the data pack. */
    QString description() const { return m_description; }

    /** Gets the image of the data pack, converted to a QPixmap for drawing, and scaled to size. */
    QPixmap image(QSize size, Qt::AspectRatioMode mode = Qt::AspectRatioMode::IgnoreAspectRatio) const;

    /** Thread-safe. */
    void setPackFormat(int new_format_id, std::pair<int, int> min_format, std::pair<int, int> max_format);

    /** Thread-safe. */
    void setDescription(QString new_description);

    /** Thread-safe. */
    void setImage(QImage new_image) const;

    bool valid() const override;

    [[nodiscard]] int compare(const Resource& other, SortType type) const override;
    [[nodiscard]] bool applyFilter(QRegularExpression filter) const override;

    QString packFormatStr() const;

   protected:
    virtual QMap<std::pair<int, int>, std::pair<Version, Version>> mappings() const;

   protected:
    mutable QMutex m_data_lock;

    /* The 'version' of a data pack, as defined in the pack.mcmeta file.
     * See https://minecraft.wiki/w/Data_pack#pack.mcmeta
     */
    int m_pack_format = 0;
    std::pair<int, int> m_min_format;
    std::pair<int, int> m_max_format;

    /** The data pack's description, as defined in the pack.mcmeta file.
     */
    QString m_description;

    /** The data pack's image file cache key, for access in the QPixmapCache global instance.
     *
     *  The 'was_ever_used' state simply identifies whether the key was never inserted on the cache (true),
     *  so as to tell whether a cache entry is inexistent or if it was just evicted from the cache.
     */
    struct {
        QPixmapCache::Key key;
        bool was_ever_used = false;
    } mutable m_pack_image_cache_key;
};
