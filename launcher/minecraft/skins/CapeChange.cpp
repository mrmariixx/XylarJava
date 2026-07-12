#include "CapeChange.h"

#include <net/DummySink.h>
#include <memory>
#include "net/RawHeaderProxy.h"

CapeChange::CapeChange(QString cape) : NetRequest(), m_capeId(cape)
{
    logCat = taskMCSkinsLogC;
}

QNetworkReply* CapeChange::getReply(QNetworkRequest& request)
{
    if (m_capeId.isEmpty()) {
        setStatus(tr("Removing cape"));
        return m_network->deleteResource(request);
    } else {
        setStatus(tr("Equipping cape"));
        return m_network->put(request, QString("{\"capeId\":\"%1\"}").arg(m_capeId).toUtf8());
    }
}

CapeChange::Ptr CapeChange::make(QString token, QString capeId)
{
    auto up = makeShared<CapeChange>(capeId);
    up->m_url = QUrl("https://api.minecraftservices.com/minecraft/profile/capes/active");
    up->setObjectName(QString("BYTES:") + up->m_url.toString());
    up->m_sink.reset(new Net::DummySink());
    up->addHeaderProxy(std::make_unique<Net::RawHeaderProxy>(QList<Net::HeaderPair>{
        { "Authorization", QString("Bearer %1").arg(token).toLocal8Bit() },
    }));
    return up;
}
