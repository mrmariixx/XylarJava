#include "java/JavaRuntimeManager.h"

#include <QDir>
#include <QFileInfo>
#include <QProcessEnvironment>
#include <QStandardPaths>

namespace xylar {

QString JavaRuntimeManager::preferredRuntimeName() const
{
    const QString java = preferredJavaExecutable();
    return java.isEmpty() ? QStringLiteral("Java not found") : java;
}

QString JavaRuntimeManager::preferredJavaExecutable() const
{
    const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    const QString javaHome = env.value(QStringLiteral("JAVA_HOME"));
    if (!javaHome.isEmpty()) {
#if defined(Q_OS_WIN)
        const QString javaw = QDir(javaHome).filePath(QStringLiteral("bin/javaw.exe"));
        if (QFileInfo::exists(javaw)) {
            return javaw;
        }
        const QString java = QDir(javaHome).filePath(QStringLiteral("bin/java.exe"));
#else
        const QString java = QDir(javaHome).filePath(QStringLiteral("bin/java"));
#endif
        if (QFileInfo::exists(java)) {
            return java;
        }
    }

#if defined(Q_OS_WIN)
    const QString javaw = QStandardPaths::findExecutable(QStringLiteral("javaw.exe"));
    if (!javaw.isEmpty()) {
        return javaw;
    }
    return QStandardPaths::findExecutable(QStringLiteral("java.exe"));
#else
    return QStandardPaths::findExecutable(QStringLiteral("java"));
#endif
}

} // namespace xylar
