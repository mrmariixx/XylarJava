#include "EnsureOfflineLibraries.h"

#include "minecraft/PackProfile.h"

EnsureOfflineLibraries::EnsureOfflineLibraries(LaunchTask* parent, MinecraftInstance* instance) : LaunchStep(parent), m_instance(instance)
{}

void EnsureOfflineLibraries::executeTask()
{
    const auto profile = m_instance->getPackProfile()->getProfile();
    QStringList allJars;
    profile->getLibraryFiles(m_instance->runtimeContext(), allJars, allJars, m_instance->getLocalLibraryPath(), m_instance->binRoot(),
                             false);

    QStringList missing;
    for (const auto& jar : allJars) {
        if (!QFileInfo::exists(jar)) {
            missing.append(jar);
        }
    }

    if (missing.isEmpty()) {
        emitSucceeded();
        return;
    }

    emit logLine("Missing libraries:", MessageLevel::Error);
    for (const auto& jar : missing) {
        emit logLine("  " + jar, MessageLevel::Error);
    }
    emit logLine(tr("\nThis instance cannot be launched because some libraries are missing or have not been downloaded yet. Please "
                    "try again in online mode with a working Internet connection"),
                 MessageLevel::Fatal);
    emitFailed("Required libraries are missing");
}
