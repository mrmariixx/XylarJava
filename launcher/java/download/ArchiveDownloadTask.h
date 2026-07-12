#pragma once

#include <QUrl>
#include "tasks/Task.h"

namespace Java {
class ArchiveDownloadTask : public Task {
    Q_OBJECT
   public:
    ArchiveDownloadTask(QUrl url, QString final_path, QString checksumType = "", QString checksumHash = "");
    virtual ~ArchiveDownloadTask() = default;

    bool canAbort() const override { return true; }
    void executeTask() override;
    virtual bool abort() override;

   private slots:
    void extractJava(QString input);

   protected:
    QUrl m_url;
    QString m_final_path;
    QString m_checksum_type;
    QString m_checksum_hash;
    Task::Ptr m_task;
};
}  // namespace Java
