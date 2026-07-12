#include "IconTheme.h"

#include <QFile>
#include <QSettings>

bool IconTheme::load()
{
    const QString path = m_path + "/index.theme";

    if (!QFile::exists(path))
        return false;

    QSettings settings(path, QSettings::IniFormat);
    settings.beginGroup("Icon Theme");
    m_name = settings.value("Name").toString();
    settings.endGroup();
    return !m_name.isNull();
}
