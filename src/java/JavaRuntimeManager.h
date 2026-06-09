#pragma once

#include <QString>

#include "download/DownloadManager.h"

namespace xylar {

class JavaRuntimeManager
{
public:
    [[nodiscard]] QString preferredRuntimeName() const;
    [[nodiscard]] QString preferredJavaExecutable() const;
    [[nodiscard]] QString bundledOracleJavaExecutable() const;
    [[nodiscard]] QString ensureOracleJdk21(DownloadManager &downloads, QString *errorMessage = nullptr) const;

private:
    [[nodiscard]] QString findJavaInDirectory(const QString &rootPath) const;
};

} // namespace xylar
