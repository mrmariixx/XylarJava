#include "MMCIcon.h"
#include <QFileInfo>
#include <QIcon>

IconType operator--(IconType& t, int)
{
    IconType temp = t;
    switch (t) {
        case IconType::Builtin:
            t = IconType::ToBeDeleted;
            break;
        case IconType::Transient:
            t = IconType::Builtin;
            break;
        case IconType::FileBased:
            t = IconType::Transient;
            break;
        default:
            break;
    }
    return temp;
}

IconType MMCIcon::type() const
{
    return m_current_type;
}

QString MMCIcon::name() const
{
    if (m_name.size())
        return m_name;
    return m_key;
}

bool MMCIcon::has(IconType _type) const
{
    return m_images[_type].present();
}

QIcon MMCIcon::icon() const
{
    if (m_current_type == IconType::ToBeDeleted)
        return QIcon();
    auto& icon = m_images[m_current_type].icon;
    if (!icon.isNull())
        return icon;
    // FIXME: inject this.
    return QIcon::fromTheme(m_images[m_current_type].key);
}

void MMCIcon::remove(IconType rm_type)
{
    m_images[rm_type].filename = QString();
    m_images[rm_type].icon = QIcon();
    for (auto iter = rm_type; iter != IconType::ToBeDeleted; iter--) {
        if (m_images[iter].present()) {
            m_current_type = iter;
            return;
        }
    }
    m_current_type = IconType::ToBeDeleted;
}

void MMCIcon::replace(IconType new_type, QIcon icon, QString path)
{
    if (new_type > m_current_type || m_current_type == IconType::ToBeDeleted) {
        m_current_type = new_type;
    }
    m_images[new_type].icon = icon;
    m_images[new_type].filename = path;
    m_images[new_type].key = QString();
}

void MMCIcon::replace(IconType new_type, const QString& key)
{
    if (new_type > m_current_type || m_current_type == IconType::ToBeDeleted) {
        m_current_type = new_type;
    }
    m_images[new_type].icon = QIcon();
    m_images[new_type].filename = QString();
    m_images[new_type].key = key;
}

QString MMCIcon::getFilePath() const
{
    if (m_current_type == IconType::ToBeDeleted) {
        return QString();
    }
    return m_images[m_current_type].filename;
}

bool MMCIcon::isBuiltIn() const
{
    return m_current_type == IconType::Builtin;
}
