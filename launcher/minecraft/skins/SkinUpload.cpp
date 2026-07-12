#include "SkinUpload.h"

#include <QHttpMultiPart>

#include "FileSystem.h"
#include "net/DummySink.h"
#include "net/RawHeaderProxy.h"

SkinUpload::SkinUpload(QString path, QString variant) : NetRequest(), m_path(path), m_variant(variant)
{
    logCat = taskMCSkinsLogC;
}

QNetworkReply* SkinUpload::getReply(QNetworkRequest& request)
{
    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType, this);

    QHttpPart skin;
    skin.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/png"));
    skin.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\"skin.png\""));

    skin.setBody(FS::read(m_path));

    QHttpPart model;
    model.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"variant\""));
    model.setBody(m_variant.toUtf8());

    multiPart->append(skin);
    multiPart->append(model);
    setStatus(tr("Uploading skin"));
    return m_network->post(request, multiPart);
}

SkinUpload::Ptr SkinUpload::make(QString token, QString path, QString variant)
{
    auto up = makeShared<SkinUpload>(path, variant);
    up->m_url = QUrl("https://api.minecraftservices.com/minecraft/profile/skins");
    up->setObjectName(QString("BYTES:") + up->m_url.toString());
    up->m_sink.reset(new Net::DummySink());
    up->addHeaderProxy(std::make_unique<Net::RawHeaderProxy>(QList<Net::HeaderPair>{
        { "Authorization", QString("Bearer %1").arg(token).toLocal8Bit() },
    }));
    return up;
}
