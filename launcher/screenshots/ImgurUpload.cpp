#include "ImgurUpload.h"
#include "BuildConfig.h"
#include "net/RawHeaderProxy.h"

#include <QDebug>
#include <QFile>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QUrl>

QNetworkReply* ImgurUpload::getReply(QNetworkRequest& request)
{
    auto file = new QFile(m_fileInfo.absoluteFilePath(), this);

    if (!file->open(QFile::ReadOnly)) {
        emitFailed(tr("Could not open file %1 for reading: %2").arg(m_fileInfo.absoluteFilePath()).arg(file->errorString()));
        return nullptr;
    }

    QHttpMultiPart* multipart = new QHttpMultiPart(QHttpMultiPart::FormDataType, this);
    file->setParent(multipart);
    QHttpPart filePart;
    filePart.setBodyDevice(file);
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, "image/png");
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"image\"; filename=\"" + file->fileName() + "\"");
    multipart->append(filePart);
    QHttpPart typePart;
    typePart.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"type\"");
    typePart.setBody("file");
    multipart->append(typePart);
    QHttpPart namePart;
    namePart.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"title\"");
    namePart.setBody(m_fileInfo.baseName().toUtf8());
    multipart->append(namePart);

    return m_network->post(request, multipart);
}

auto ImgurUpload::Sink::init(QNetworkRequest& request) -> Task::State
{
    m_output.clear();
    return Task::State::Running;
}

auto ImgurUpload::Sink::write(QByteArray& data) -> Task::State
{
    m_output.append(data);
    return Task::State::Running;
}

auto ImgurUpload::Sink::abort() -> Task::State
{
    m_output.clear();
    m_fail_reason = "Aborted";
    return Task::State::Failed;
}

auto ImgurUpload::Sink::finalize(QNetworkReply&) -> Task::State
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(m_output, &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        qDebug() << "imgur server did not reply with JSON" << jsonError.errorString();
        m_fail_reason = "Invalid json reply";
        return Task::State::Failed;
    }
    auto object = doc.object();
    if (!object.value("success").toBool()) {
        qDebug() << "Screenshot upload not successful:" << doc.toJson();
        m_fail_reason = "Screenshot was not uploaded successfully";
        return Task::State::Failed;
    }
    m_shot->m_imgurId = object.value("data").toObject().value("id").toString();
    m_shot->m_url = object.value("data").toObject().value("link").toString();
    m_shot->m_imgurDeleteHash = object.value("data").toObject().value("deletehash").toString();
    return Task::State::Succeeded;
}

Net::NetRequest::Ptr ImgurUpload::make(ScreenShot::Ptr m_shot)
{
    auto up = makeShared<ImgurUpload>(m_shot->m_file);
    up->m_url = std::move(BuildConfig.IMGUR_BASE_URL + "image");
    up->m_sink.reset(new Sink(m_shot));
    up->addHeaderProxy(std::make_unique<Net::RawHeaderProxy>(QList<Net::HeaderPair>{
        { "Authorization", QString("Client-ID %1").arg(BuildConfig.IMGUR_CLIENT_ID).toUtf8() }, { "Accept", "application/json" } }));
    return up;
}
