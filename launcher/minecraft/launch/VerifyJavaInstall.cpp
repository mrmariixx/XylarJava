#include "VerifyJavaInstall.h"
#include <memory>

#include "Application.h"
#include "MessageLevel.h"
#include "java/JavaInstall.h"
#include "java/JavaInstallList.h"
#include "java/JavaVersion.h"
#include "minecraft/MinecraftInstance.h"
#include "minecraft/PackProfile.h"

void VerifyJavaInstall::executeTask()
{
    auto instance = m_parent->instance();
    auto packProfile = instance->getPackProfile();
    auto settings = instance->settings();
    auto storedVersion = settings->get("JavaVersion").toString();
    auto ignoreCompatibility = settings->get("IgnoreJavaCompatibility").toBool();
    auto javaArchitecture = settings->get("JavaArchitecture").toString();
    auto maxMemAlloc = settings->get("MaxMemAlloc").toInt();

    if (javaArchitecture == "32" && maxMemAlloc > 2048) {
        emit logLine(tr("Max memory allocation exceeds the supported value.\n"
                        "The selected installation of Java is 32-bit and doesn't support more than 2048MiB of RAM.\n"
                        "The instance may not start due to this."),
                     MessageLevel::Error);
    }

    auto compatibleMajors = packProfile->getProfile()->getCompatibleJavaMajors();

    JavaVersion javaVersion(storedVersion);

    if (compatibleMajors.isEmpty() || compatibleMajors.contains(javaVersion.major())) {
        emitSucceeded();
        return;
    }

    if (ignoreCompatibility) {
        emit logLine(tr("Java major version is incompatible. Things might break.\n"), MessageLevel::Warning);
        emitSucceeded();
        return;
    }

    emit logLine(tr("This instance is not compatible with Java version %1.\n"
                    "Please switch to one of the following Java versions for this instance:")
                     .arg(javaVersion.major()),
                 MessageLevel::Error);
    for (auto major : compatibleMajors) {
        emit logLine(tr("Java version %1").arg(major), MessageLevel::Error);
    }
    emit logLine(tr("Go to instance Java settings to change your Java version or disable the Java compatibility check if you know what "
                    "you're doing."),
                 MessageLevel::Error);

    emitFailed(QString("Incompatible Java major version"));
}
