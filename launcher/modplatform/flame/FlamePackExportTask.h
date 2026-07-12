#pragma once

#include "MMCZip.h"
#include "minecraft/MinecraftInstance.h"
#include "modplatform/flame/FlameAPI.h"
#include "tasks/Task.h"

struct FlamePackExportOptions {
    QString name;
    QString version;
    QString author;
    bool optionalFiles;
    MinecraftInstance* instance;
    QString output;
    MMCZip::FilterFileFunction filter;
    int recommendedRAM;
};

class FlamePackExportTask : public Task {
    Q_OBJECT
   public:
    FlamePackExportTask(FlamePackExportOptions&& options);

   protected:
    void executeTask() override;
    bool abort() override;

   private:
    static const QString TEMPLATE;
    static const QStringList FILE_EXTENSIONS;

    // inputs

    struct ResolvedFile {
        int addonId;
        int version;
        bool enabled;
        bool isMod;

        QString name;
        QString slug;
        QString authors;
    };
    struct HashInfo {
        QString name;
        QString path;
        bool enabled;
        bool isMod;
    };

    FlamePackExportOptions m_options;
    QDir m_gameRoot;

    FlameAPI api;

    QFileInfoList m_files;
    QMap<QString, HashInfo> pendingHashes{};
    QMap<QString, ResolvedFile> resolvedFiles{};
    Task::Ptr task;

    void collectFiles();
    void collectHashes();
    void makeApiRequest();
    void getProjectsInfo();
    void buildZip();

    QByteArray generateIndex();
    QByteArray generateHTML();
};
