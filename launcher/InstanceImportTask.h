#pragma once

#include <QFuture>
#include <QFutureWatcher>
#include <QUrl>
#include "InstanceTask.h"

class InstanceImportTask : public InstanceTask {
    Q_OBJECT
   public:
    explicit InstanceImportTask(const QUrl& sourceUrl, QWidget* parent = nullptr, QMap<QString, QString>&& extra_info = {});
    virtual ~InstanceImportTask() = default;
    bool abort() override;

   protected:
    //! Entry point for tasks.
    virtual void executeTask() override;

   private:
    void processMultiMC();
    void processTechnic();
    void processFlame();
    void processModrinth();

   private slots:
    void processZipPack();
    void extractFinished();

   private: /* data */
    QUrl m_sourceUrl;
    QString m_archivePath;
    Task::Ptr m_task;
    enum class ModpackType {
        Unknown,
        MultiMC,
        Technic,
        Flame,
        Modrinth,
    } m_modpackType = ModpackType::Unknown;

    // Extra info we might need, that's available before, but can't be derived from
    // the source URL / the resource it points to alone.
    QMap<QString, QString> m_extra_info;

    // FIXME: nuke
    QWidget* m_parent;
    void downloadFromUrl();
};
