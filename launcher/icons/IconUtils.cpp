#include "IconUtils.h"

#include <QDirIterator>
#include "FileSystem.h"

namespace {
static const QStringList validIconExtensions = { { "svg", "png", "ico", "gif", "jpg", "jpeg", "webp" } };
}

namespace IconUtils {

QString findBestIconIn(const QString& folder, const QString& iconKey)
{
    QString best_filename;

    QDirIterator it(folder, QDir::NoDotAndDotDot | QDir::Files, QDirIterator::NoIteratorFlags);
    while (it.hasNext()) {
        it.next();
        auto fileInfo = it.fileInfo();
        if ((fileInfo.completeBaseName() == iconKey || fileInfo.fileName() == iconKey) && isIconSuffix(fileInfo.suffix()))
            return fileInfo.absoluteFilePath();
    }
    return {};
}

QString getIconFilter()
{
    return "(*." + validIconExtensions.join(" *.") + ")";
}

bool isIconSuffix(QString suffix)
{
    return validIconExtensions.contains(suffix);
}

}  // namespace IconUtils
