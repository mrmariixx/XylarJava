#pragma once

#include <InstanceTask.h>
#include <net/NetJob.h>
#include <tasks/Task.h>

#include <QUrl>

namespace Technic {
class SolderPackInstallTask : public InstanceTask {
    Q_OBJECT
   public:
    explicit SolderPackInstallTask(QNetworkAccessManager* network,
                                   const QUrl& solderUrl,
                                   const QString& pack,
                                   const QString& version,
                                   const QString& minecraftVersion);

    bool canAbort() const override { return true; }
    bool abort() override;

   protected:
    //! Entry point for tasks.
    virtual void executeTask() override;

   private slots:
    void fileListSucceeded(QByteArray* response);
    void downloadSucceeded();
    void downloadFailed(QString reason);
    void downloadProgressChanged(qint64 current, qint64 total);
    void downloadAborted();
    void extractFinished();
    void extractAborted();

   private:
    bool m_abortable = false;

    QNetworkAccessManager* m_network;

    NetJob::Ptr m_filesNetJob;
    QUrl m_solderUrl;
    QString m_pack;
    QString m_version;
    QString m_minecraftVersion;
    QTemporaryDir m_outputDir;
    int m_modCount;
    QFuture<bool> m_extractFuture;
    QFutureWatcher<bool> m_extractFutureWatcher;
};
}  // namespace Technic
