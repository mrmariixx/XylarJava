#include "java/JavaRuntimeManager.h"

#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QProcessEnvironment>
#include <QStandardPaths>

#include "fs/Paths.h"

namespace xylar {
namespace {

const QUrl kOracleJdk21WindowsZip(QStringLiteral("https://download.oracle.com/java/21/latest/jdk-21_windows-x64_bin.zip"));

QString quoteForPowerShell(QString value)
{
    value.replace(QStringLiteral("'"), QStringLiteral("''"));
    return QStringLiteral("'") + value + QStringLiteral("'");
}

} // namespace

QString JavaRuntimeManager::preferredRuntimeName() const
{
    const QString java = preferredJavaExecutable();
    return java.isEmpty() ? QStringLiteral("Java not found") : java;
}

QString JavaRuntimeManager::preferredJavaExecutable() const
{
    const QString bundled = bundledOracleJavaExecutable();
    if (!bundled.isEmpty()) {
        return bundled;
    }

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

QString JavaRuntimeManager::bundledOracleJavaExecutable() const
{
    return findJavaInDirectory(Paths::runtimesDir().filePath(QStringLiteral("oracle-jdk-21")));
}

QString JavaRuntimeManager::ensureOracleJdk21(DownloadManager &downloads, QString *errorMessage) const
{
    const QString existing = bundledOracleJavaExecutable();
    if (!existing.isEmpty()) {
        return existing;
    }

#if !defined(Q_OS_WIN)
    if (errorMessage) {
        *errorMessage = QStringLiteral("Automatic Oracle JDK 21 download is currently wired for Windows.");
    }
    return {};
#else
    const QString root = Paths::runtimesDir().filePath(QStringLiteral("oracle-jdk-21"));
    const QString archive = Paths::runtimesDir().filePath(QStringLiteral("oracle-jdk-21.zip"));
    QDir().mkpath(root);

    DownloadRequest request;
    request.url = kOracleJdk21WindowsZip;
    request.targetPath = archive;
    request.label = QStringLiteral("Oracle JDK 21");
    request.force = true;
    downloads.beginBatch(1);

    QString error;
    if (!downloads.downloadToFile(request, &error)) {
        if (errorMessage) {
            *errorMessage = error;
        }
        return {};
    }

    QDir extractDir(root);
    if (extractDir.exists()) {
        extractDir.removeRecursively();
    }
    QDir().mkpath(root);

    const QString script = QStringLiteral(
        "Expand-Archive -LiteralPath %1 -DestinationPath %2 -Force"
    ).arg(quoteForPowerShell(QDir::toNativeSeparators(archive)), quoteForPowerShell(QDir::toNativeSeparators(root)));

    QProcess process;
    process.start(QStringLiteral("powershell.exe"), {
        QStringLiteral("-NoProfile"),
        QStringLiteral("-ExecutionPolicy"),
        QStringLiteral("Bypass"),
        QStringLiteral("-Command"),
        script,
    });
    if (!process.waitForFinished(300000) || process.exitCode() != 0) {
        if (errorMessage) {
            const QString output = QString::fromLocal8Bit(process.readAllStandardError() + process.readAllStandardOutput()).trimmed();
            *errorMessage = output.isEmpty()
                ? QStringLiteral("Cannot extract Oracle JDK 21.")
                : QStringLiteral("Cannot extract Oracle JDK 21: %1").arg(output);
        }
        return {};
    }

    const QString java = bundledOracleJavaExecutable();
    if (java.isEmpty() && errorMessage) {
        *errorMessage = QStringLiteral("Oracle JDK 21 was extracted, but javaw.exe was not found.");
    }
    return java;
#endif
}

QString JavaRuntimeManager::findJavaInDirectory(const QString &rootPath) const
{
    QDir root(rootPath);
    if (!root.exists()) {
        return {};
    }

#if defined(Q_OS_WIN)
    const QStringList names = {QStringLiteral("javaw.exe"), QStringLiteral("java.exe")};
#else
    const QStringList names = {QStringLiteral("java")};
#endif
    for (const QString &name : names) {
        const QFileInfoList matches = root.entryInfoList({name}, QDir::Files, QDir::Name);
        if (!matches.isEmpty()) {
            return matches.first().absoluteFilePath();
        }
    }

    const QFileInfoList dirs = root.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    for (const QFileInfo &dir : dirs) {
        const QString found = findJavaInDirectory(dir.absoluteFilePath());
        if (!found.isEmpty()) {
            return found;
        }
    }
    return {};
}

} // namespace xylar
