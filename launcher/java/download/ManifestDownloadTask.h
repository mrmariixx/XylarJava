#pragma once

#include <QUrl>
#include "tasks/Task.h"

namespace Java {

class ManifestDownloadTask : public Task {
    Q_OBJECT
   public:
    ManifestDownloadTask(QUrl url, QString final_path, QString checksumType = "", QString checksumHash = "");
    virtual ~ManifestDownloadTask() = default;

    bool canAbort() const override { return true; }
    void executeTask() override;
    virtual bool abort() override;

   private slots:
    void downloadJava(const QJsonDocument& doc);

   protected:
    QUrl m_url;
    QString m_final_path;
    QString m_checksum_type;
    QString m_checksum_hash;
    Task::Ptr m_task;
};
}  // namespace Java
