#pragma once

#include <QDebug>
#include <QObject>

#include "minecraft/mod/TexturePack.h"

#include "tasks/Task.h"

namespace TexturePackUtils {

enum class ProcessingLevel { Full, BasicInfoOnly };

bool process(TexturePack& pack, ProcessingLevel level = ProcessingLevel::Full);

bool processZIP(TexturePack& pack, ProcessingLevel level = ProcessingLevel::Full);
bool processFolder(TexturePack& pack, ProcessingLevel level = ProcessingLevel::Full);

bool processPackTXT(TexturePack& pack, QByteArray&& raw_data);
bool processPackPNG(const TexturePack& pack, QByteArray&& raw_data);

/// processes ONLY the pack.png (rest of the pack may be invalid)
bool processPackPNG(const TexturePack& pack);

/** Checks whether a file is valid as a texture pack or not. */
bool validate(QFileInfo file);
}  // namespace TexturePackUtils

class LocalTexturePackParseTask : public Task {
    Q_OBJECT
   public:
    LocalTexturePackParseTask(int token, TexturePack& rp);

    bool canAbort() const override { return true; }
    bool abort() override;

    void executeTask() override;

    int token() const { return m_token; }

   private:
    int m_token;

    TexturePack& m_texture_pack;

    bool m_aborted = false;
};
