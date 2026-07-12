#pragma once
#include <QObject>
#include <QTimer>

#include "minecraft/auth/AuthStep.h"
#include "net/NetJob.h"
#include "net/Upload.h"

class MSADeviceCodeStep : public AuthStep {
    Q_OBJECT
   public:
    explicit MSADeviceCodeStep(AccountData* data);
    virtual ~MSADeviceCodeStep() noexcept = default;

    void perform() override;

    QString describe() override;

   public slots:
    void abort() override;

   signals:
    void authorizeWithBrowser(QString url, QString code, int expiresIn);

   private slots:
    void deviceAuthorizationFinished(QByteArray* response);
    void startPoolTimer();
    void authenticateUser();
    void authenticationFinished(QByteArray* response);

   private:
    QString m_clientId;
    QString m_device_code;
    bool m_is_aborted = false;
    int interval = 5;

    QTimer m_pool_timer;
    QTimer m_expiration_timer;

    Net::Upload::Ptr m_request;
    NetJob::Ptr m_task;
};
