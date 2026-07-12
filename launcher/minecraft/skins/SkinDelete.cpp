#include "SkinDelete.h"

#include <net/DummySink.h>
#include "net/RawHeaderProxy.h"

SkinDelete::SkinDelete() : NetRequest()
{
    logCat = taskMCSkinsLogC;
}

QNetworkReply* SkinDelete::getReply(QNetworkRequest& request)
{
    setStatus(tr("Deleting skin"));
    return m_network->deleteResource(request);
}

SkinDelete::Ptr SkinDelete::make(QString token)
{
    auto up = makeShared<SkinDelete>();
    up->m_url = QUrl("https://api.minecraftservices.com/minecraft/profile/skins/active");
    up->m_sink.reset(new Net::DummySink());
    up->addHeaderProxy(std::make_unique<Net::RawHeaderProxy>(QList<Net::HeaderPair>{
        { "Authorization", QString("Bearer %1").arg(token).toLocal8Bit() },
    }));
    return up;
}
