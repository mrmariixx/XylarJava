#include "EntitlementsStep.h"

#include <QList>
#include <QNetworkRequest>
#include <QUrl>
#include <memory>

#include "Application.h"
#include "Logging.h"
#include "minecraft/auth/Parsers.h"
#include "net/Download.h"
#include "net/NetJob.h"
#include "net/NetUtils.h"
#include "net/RawHeaderProxy.h"
#include "tasks/Task.h"

EntitlementsStep::EntitlementsStep(AccountData* data) : AuthStep(data) {}

QString EntitlementsStep::describe()
{
    return tr("Determining game ownership.");
}

void EntitlementsStep::perform()
{
    QUrl url("https://api.minecraftservices.com/entitlements/mcstore");
    auto headers = QList<Net::HeaderPair>{ { "Content-Type", "application/json" },
                                           { "Accept", "application/json" },
                                           { "Authorization", QString("Bearer %1").arg(m_data->yggdrasilToken.token).toUtf8() } };

    auto [request, response] = Net::Download::makeByteArray(url);
    m_request = request;
    m_request->addHeaderProxy(std::make_unique<Net::RawHeaderProxy>(headers));
    m_request->enableAutoRetry(true);

    m_task.reset(new NetJob("EntitlementsStep", APPLICATION->network()));
    m_task->setAskRetry(false);
    m_task->addNetAction(m_request);

    connect(m_task.get(), &Task::finished, this, [this, response] { onRequestDone(response); });

    m_task->start();
    qDebug() << "Getting entitlements...";
}

void EntitlementsStep::onRequestDone(QByteArray* response)
{
    qCDebug(authCredentials()) << *response;

    if (m_request->error() != QNetworkReply::NoError) {
        qWarning() << "Entitlements request failed:" << m_request->errorString();
        if (Net::isApplicationError(m_request->error())) {
            emit finished(AccountTaskState::STATE_FAILED_SOFT,
                          tr("Failed to verify Minecraft ownership: %1").arg(m_request->errorString()));
        } else {
            emit finished(AccountTaskState::STATE_OFFLINE, tr("Failed to verify Minecraft ownership: %1").arg(m_request->errorString()));
        }
        return;
    }

    if (!Parsers::parseMinecraftEntitlements(*response, m_data->minecraftEntitlement)) {
        emit finished(AccountTaskState::STATE_FAILED_SOFT, tr("Failed to parse Minecraft ownership response."));
        return;
    }

    if (!m_data->minecraftEntitlement.canPlayMinecraft && !m_data->minecraftEntitlement.ownsMinecraft) {
        emit finished(AccountTaskState::STATE_FAILED_SOFT,
                      tr("This Microsoft account does not own Minecraft Java Edition. Purchase the game on "
                         "<a href=\"https://www.minecraft.net/en-us/store/minecraft-java-edition\">minecraft.net</a> first."));
        return;
    }

    emit finished(AccountTaskState::STATE_WORKING, tr("Minecraft ownership verified"));
}
