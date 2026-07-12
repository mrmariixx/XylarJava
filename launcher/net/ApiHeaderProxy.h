#pragma once

#include "Application.h"
#include "BuildConfig.h"
#include "net/HeaderProxy.h"

namespace Net {

class ApiHeaderProxy : public HeaderProxy {
   public:
    ApiHeaderProxy() : HeaderProxy() {}
    virtual ~ApiHeaderProxy() = default;

   public:
    virtual QList<HeaderPair> headers(const QNetworkRequest& request) const override
    {
        QList<HeaderPair> hdrs;
        if (APPLICATION->capabilities() & Application::SupportsFlame && request.url().host() == QUrl(BuildConfig.FLAME_BASE_URL).host()) {
            hdrs.append({ "x-api-key", APPLICATION->getFlameAPIKey().toUtf8() });
        } else if (request.url().host() == QUrl(BuildConfig.MODRINTH_PROD_URL).host() ||
                   request.url().host() == QUrl(BuildConfig.MODRINTH_STAGING_URL).host()) {
            QString token = APPLICATION->getModrinthAPIToken();
            if (!token.isNull())
                hdrs.append({ "Authorization", token.toUtf8() });
        }
        return hdrs;
    };
};

}  // namespace Net
