#include "ScanModFolders.h"
#include "FileSystem.h"
#include "MMCZip.h"
#include "launch/LaunchTask.h"
#include "minecraft/MinecraftInstance.h"
#include "minecraft/mod/ModFolderModel.h"

void ScanModFolders::executeTask()
{
    auto m_inst = m_parent->instance();

    auto loaders = m_inst->loaderModList();
    connect(loaders, &ModFolderModel::updateFinished, this, &ScanModFolders::modsDone);
    if (!loaders->update()) {
        m_modsDone = true;
    }

    auto cores = m_inst->coreModList();
    connect(cores, &ModFolderModel::updateFinished, this, &ScanModFolders::coreModsDone);
    if (!cores->update()) {
        m_coreModsDone = true;
    }

    auto nils = m_inst->nilModList();
    connect(nils, &ModFolderModel::updateFinished, this, &ScanModFolders::nilModsDone);
    if (!nils->update()) {
        m_nilModsDone = true;
    }
    checkDone();
}

void ScanModFolders::modsDone()
{
    m_modsDone = true;
    checkDone();
}

void ScanModFolders::coreModsDone()
{
    m_coreModsDone = true;
    checkDone();
}

void ScanModFolders::nilModsDone()
{
    m_nilModsDone = true;
    checkDone();
}

void ScanModFolders::checkDone()
{
    if (m_modsDone && m_coreModsDone && m_nilModsDone) {
        emitSucceeded();
    }
}
