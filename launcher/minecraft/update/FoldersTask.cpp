#include "FoldersTask.h"
#include <QDir>
#include "minecraft/MinecraftInstance.h"

FoldersTask::FoldersTask(MinecraftInstance* inst)
{
    m_inst = inst;
}

void FoldersTask::executeTask()
{
    // Make directories
    QDir mcDir(m_inst->gameRoot());
    if (!mcDir.exists() && !mcDir.mkpath(".")) {
        emitFailed(tr("Failed to create folder for Minecraft binaries."));
        return;
    }
    emitSucceeded();
}
