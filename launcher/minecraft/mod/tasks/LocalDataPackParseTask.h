#pragma once

#include <QDebug>
#include <QObject>

#include "minecraft/mod/DataPack.h"

#include "tasks/Task.h"

namespace DataPackUtils {

enum class ProcessingLevel { Full, BasicInfoOnly };

bool process(DataPack* pack, ProcessingLevel level = ProcessingLevel::Full);

bool processZIP(DataPack* pack, ProcessingLevel level = ProcessingLevel::Full);
bool processFolder(DataPack* pack, ProcessingLevel level = ProcessingLevel::Full);

bool processMCMeta(DataPack* pack, QByteArray&& raw_data);

QString processComponent(const QJsonValue& value, bool strikethrough = false, bool underline = false);

bool processPackPNG(const DataPack* pack, QByteArray&& raw_data);

/// processes ONLY the pack.png (rest of the pack may be invalid)
bool processPackPNG(const DataPack* pack);

/** Checks whether a file is valid as a data pack or not. */
bool validate(QFileInfo file);

/** Checks whether a file is valid as a resource pack or not. */
bool validateResourcePack(QFileInfo file);

}  // namespace DataPackUtils

class LocalDataPackParseTask : public Task {
    Q_OBJECT
   public:
    LocalDataPackParseTask(int token, DataPack* dp);

    void executeTask() override;

    int token() const { return m_token; }

   private:
    int m_token;

    DataPack* m_data_pack;
};
