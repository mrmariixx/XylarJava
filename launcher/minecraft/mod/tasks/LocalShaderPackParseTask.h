#pragma once

#include <QDebug>
#include <QObject>

#include "minecraft/mod/ShaderPack.h"

#include "tasks/Task.h"

namespace ShaderPackUtils {

enum class ProcessingLevel { Full, BasicInfoOnly };

bool process(ShaderPack& pack, ProcessingLevel level = ProcessingLevel::Full);

bool processZIP(ShaderPack& pack, ProcessingLevel level = ProcessingLevel::Full);
bool processFolder(ShaderPack& pack, ProcessingLevel level = ProcessingLevel::Full);

/** Checks whether a file is valid as a shader pack or not. */
bool validate(QFileInfo file);
}  // namespace ShaderPackUtils

class LocalShaderPackParseTask : public Task {
    Q_OBJECT
   public:
    LocalShaderPackParseTask(int token, ShaderPack& sp);

    bool canAbort() const override { return true; }
    bool abort() override;

    void executeTask() override;

    int token() const { return m_token; }

   private:
    int m_token;

    ShaderPack& m_shader_pack;

    bool m_aborted = false;
};
