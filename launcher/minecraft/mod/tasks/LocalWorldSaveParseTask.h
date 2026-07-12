#pragma once

#include <QDebug>
#include <QObject>

#include "minecraft/mod/WorldSave.h"

#include "tasks/Task.h"

namespace WorldSaveUtils {

enum class ProcessingLevel { Full, BasicInfoOnly };

bool process(WorldSave& save, ProcessingLevel level = ProcessingLevel::Full);

bool processZIP(WorldSave& pack, ProcessingLevel level = ProcessingLevel::Full);
bool processFolder(WorldSave& pack, ProcessingLevel level = ProcessingLevel::Full);

bool validate(QFileInfo file);

}  // namespace WorldSaveUtils

class LocalWorldSaveParseTask : public Task {
    Q_OBJECT
   public:
    LocalWorldSaveParseTask(int token, WorldSave& save);

    bool canAbort() const override { return true; }
    bool abort() override;

    void executeTask() override;

    int token() const { return m_token; }

   private:
    int m_token;

    WorldSave& m_save;

    bool m_aborted = false;
};
