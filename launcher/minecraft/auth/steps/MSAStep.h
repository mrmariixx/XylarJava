#pragma once
#include <QObject>

#include "minecraft/auth/AuthStep.h"

#include <QtNetworkAuth/qoauth2authorizationcodeflow.h>
class MSAStep : public AuthStep {
    Q_OBJECT
   public:
    explicit MSAStep(AccountData* data, bool silent = false);
    virtual ~MSAStep() noexcept = default;

    void perform() override;

    QString describe() override;

   signals:
    void authorizeWithBrowser(const QUrl& url);

   private:
    bool m_silent;
    QString m_clientId;
    QOAuth2AuthorizationCodeFlow m_oauth2;
};
