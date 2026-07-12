#include "LauncherLoginStep.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QUrl>

#include "Application.h"
#include "Logging.h"
#include "minecraft/auth/Parsers.h"
#include "net/NetUtils.h"
#include "net/RawHeaderProxy.h"
#include "net/Upload.h"

namespace {
QString buildIdentityToken(const QString& uhs, const QString& xToken)
{
    return QStringLiteral("XBL3.0 x=%1;%2").arg(uhs, xToken);
}

QString parseMinecraftAuthError(const QByteArray& response)
{
    QJsonParseError jsonError;
    const QJsonDocument doc = QJsonDocument::fromJson(response, &jsonError);
    if (jsonError.error != QJsonParseError::NoError || !doc.isObject()) {
        return {};
    }
    const auto obj = doc.object();
    const QString errorMessage = obj.value("errorMessage").toString();
    const QString errorCode = obj.value("errorCode").toString();
    if (!errorMessage.isEmpty()) {
        if (!errorCode.isEmpty()) {
            return QStringLiteral("%1 (%2)").arg(errorMessage, errorCode);
        }
        return errorMessage;
    }
    return obj.value("error").toString();
}
}  // namespace

LauncherLoginStep::LauncherLoginStep(AccountData* data) : AuthStep(data) {}

QString LauncherLoginStep::describe()
{
    return tr("Fetching Minecraft access token");
}

void LauncherLoginStep::perform()
{
    startRequest(APPLICATION->useMSALauncherLoginEndpoint());
}

void LauncherLoginStep::startRequest(bool useLauncherEndpoint)
{
    m_useLauncherEndpoint = useLauncherEndpoint;

    const auto uhs = m_data->mojangservicesToken.extra["uhs"].toString();
    const auto xToken = m_data->mojangservicesToken.token;
    if (uhs.isEmpty() || xToken.isEmpty()) {
        emit finished(AccountTaskState::STATE_FAILED_SOFT,
                      tr("Missing Xbox authorization data. The Xbox Live step did not return a valid token."));
        return;
    }

    const QString identityToken = buildIdentityToken(uhs, xToken);
    QJsonObject body;
    QUrl url;
    if (m_useLauncherEndpoint) {
        url = QUrl("https://api.minecraftservices.com/launcher/login");
        body["xtoken"] = identityToken;
        body["platform"] = "PC_LAUNCHER";
    } else {
        url = QUrl("https://api.minecraftservices.com/authentication/login_with_xbox");
        body["identityToken"] = identityToken;
    }

    const QByteArray requestBody = QJsonDocument(body).toJson(QJsonDocument::Compact);
    auto headers = QList<Net::HeaderPair>{
        { "Content-Type", "application/json" },
        { "Accept", "application/json" },
    };

    auto [request, response] = Net::Upload::makeByteArray(url, requestBody);
    m_request = request;
    m_request->addHeaderProxy(std::make_unique<Net::RawHeaderProxy>(headers));
    m_request->enableAutoRetry(true);

    m_task.reset(new NetJob("LauncherLoginStep", APPLICATION->network()));
    m_task->setAskRetry(false);
    m_task->addNetAction(m_request);

    connect(m_task.get(), &Task::finished, this, [this, response] { onRequestDone(response); });

    m_task->start();
    qDebug() << "Getting Minecraft access token via" << url.toString();
}

void LauncherLoginStep::onRequestDone(QByteArray* response)
{
    qCDebug(authCredentials()) << *response;

    if (m_request->error() != QNetworkReply::NoError) {
        const int status = m_request->replyStatusCode();
        const QString apiError = parseMinecraftAuthError(*response);

        if (status == 403 && !m_useLauncherEndpoint && !m_triedFallback) {
            m_triedFallback = true;
            qWarning() << "login_with_xbox returned 403, retrying with launcher/login endpoint";
            startRequest(true);
            return;
        }

        qWarning() << "Reply error:" << m_request->error() << "HTTP" << status << apiError;

        QString message = tr("Failed to get Minecraft access token");
        if (!apiError.isEmpty()) {
            message += QStringLiteral(": %1").arg(apiError);
        } else {
            message += QStringLiteral(": %1").arg(m_request->errorString());
        }

        if (status == 403) {
            message += QStringLiteral("\n\n%1")
                            .arg(tr("This usually means the Microsoft account does not own Minecraft Java Edition, "
                                    "or the Azure client ID in auth-settings.ini is not authorized for Minecraft login."));
        }

        if (Net::isApplicationError(m_request->error())) {
            emit finished(AccountTaskState::STATE_FAILED_SOFT, message);
        } else {
            emit finished(AccountTaskState::STATE_OFFLINE, message);
        }
        return;
    }

    if (!Parsers::parseMojangResponse(*response, m_data->yggdrasilToken)) {
        qWarning() << "Could not parse Minecraft login response...";
        emit finished(AccountTaskState::STATE_FAILED_SOFT, tr("Failed to parse the Minecraft access token response."));
        return;
    }
    emit finished(AccountTaskState::STATE_WORKING, tr("Got Minecraft access token"));
}
