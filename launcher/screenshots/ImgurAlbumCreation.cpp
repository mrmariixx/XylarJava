#include "ImgurAlbumCreation.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QNetworkRequest>
#include <QStringList>
#include <QUrl>
#include <memory>

#include "BuildConfig.h"
#include "net/RawHeaderProxy.h"

Net::NetRequest::Ptr ImgurAlbumCreation::make(std::shared_ptr<ImgurAlbumCreation::Result> output, QList<ScreenShot::Ptr> screenshots)
{
    auto up = makeShared<ImgurAlbumCreation>();
    up->m_url = BuildConfig.IMGUR_BASE_URL + "album";
    up->m_sink.reset(new Sink(output));
    up->m_screenshots = screenshots;
    up->addHeaderProxy(std::make_unique<Net::RawHeaderProxy>(
        QList<Net::HeaderPair>{ { "Content-Type", "application/x-www-form-urlencoded" },
                                { "Authorization", QString("Client-ID %1").arg(BuildConfig.IMGUR_CLIENT_ID).toUtf8() },
                                { "Accept", "application/json" } }));
    return up;
}

QNetworkReply* ImgurAlbumCreation::getReply(QNetworkRequest& request)
{
    QStringList hashes;
    for (auto shot : m_screenshots) {
        hashes.append(shot->m_imgurDeleteHash);
    }
    const QByteArray data = "deletehashes=" + hashes.join(',').toUtf8() + "&title=Minecraft%20Screenshots&privacy=hidden";
    return m_network->post(request, data);
}

auto ImgurAlbumCreation::Sink::init(QNetworkRequest& request) -> Task::State
{
    m_output.clear();
    return Task::State::Running;
}

auto ImgurAlbumCreation::Sink::write(QByteArray& data) -> Task::State
{
    m_output.append(data);
    return Task::State::Running;
}

auto ImgurAlbumCreation::Sink::abort() -> Task::State
{
    m_output.clear();
    m_fail_reason = "Aborted";
    return Task::State::Failed;
}

auto ImgurAlbumCreation::Sink::finalize(QNetworkReply&) -> Task::State
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(m_output, &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        qDebug() << jsonError.errorString();
        m_fail_reason = "Invalid json reply";
        return Task::State::Failed;
    }
    auto object = doc.object();
    if (!object.value("success").toBool()) {
        qDebug() << doc.toJson();
        m_fail_reason = "Failed to create album";
        return Task::State::Failed;
    }
    m_result->deleteHash = object.value("data").toObject().value("deletehash").toString();
    m_result->id = object.value("data").toObject().value("id").toString();
    return Task::State::Succeeded;
}
