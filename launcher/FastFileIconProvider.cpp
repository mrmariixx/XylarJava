#include "FastFileIconProvider.h"

#include <QApplication>
#include <QStyle>

QIcon FastFileIconProvider::icon(const QFileInfo& info) const
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
    bool link = info.isSymbolicLink() || info.isAlias() || info.isShortcut();
#else
    // in versions prior to 6.4 we don't have access to isAlias
    bool link = info.isSymLink();
#endif
    QStyle::StandardPixmap icon;

    if (info.isDir()) {
        if (link)
            icon = QStyle::SP_DirLinkIcon;
        else
            icon = QStyle::SP_DirIcon;
    } else {
        if (link)
            icon = QStyle::SP_FileLinkIcon;
        else
            icon = QStyle::SP_FileIcon;
    }

    return QApplication::style()->standardIcon(icon);
}
