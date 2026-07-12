#include "ModMinecraftJar.h"
#include "FileSystem.h"
#include "MMCZip.h"
#include "launch/LaunchTask.h"
#include "minecraft/MinecraftInstance.h"
#include "minecraft/PackProfile.h"

void ModMinecraftJar::executeTask()
{
    auto m_inst = m_parent->instance();

    if (!m_inst->getJarMods().size()) {
        emitSucceeded();
        return;
    }
    // nuke obsolete stripped jar(s) if needed
    if (!FS::ensureFolderPathExists(m_inst->binRoot())) {
        emitFailed(tr("Couldn't create the bin folder for Minecraft.jar"));
        return;
    }

    auto finalJarPath = QDir(m_inst->binRoot()).absoluteFilePath("minecraft.jar");
    if (!removeJar()) {
        emitFailed(tr("Couldn't remove stale jar file: %1").arg(finalJarPath));
        return;
    }

    // create temporary modded jar, if needed
    auto components = m_inst->getPackProfile();
    auto profile = components->getProfile();
    auto jarMods = m_inst->getJarMods();
    if (jarMods.size()) {
        auto mainJar = profile->getMainJar();
        QStringList jars, temp1, temp2, temp3, temp4;
        mainJar->getApplicableFiles(m_inst->runtimeContext(), jars, temp1, temp2, temp3, m_inst->getLocalLibraryPath());
        auto sourceJarPath = jars[0];
        if (!MMCZip::createModdedJar(sourceJarPath, finalJarPath, jarMods)) {
            emitFailed(tr("Failed to create the custom Minecraft jar file."));
            return;
        }
    }
    emitSucceeded();
}

void ModMinecraftJar::finalize()
{
    removeJar();
}

bool ModMinecraftJar::removeJar()
{
    auto m_inst = m_parent->instance();
    auto finalJarPath = QDir(m_inst->binRoot()).absoluteFilePath("minecraft.jar");
    QFile finalJar(finalJarPath);
    if (finalJar.exists()) {
        if (!finalJar.remove()) {
            return false;
        }
    }
    return true;
}
