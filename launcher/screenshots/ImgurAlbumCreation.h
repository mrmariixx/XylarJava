#pragma once

#include "Screenshot.h"
#include "net/NetRequest.h"

class ImgurAlbumCreation : public Net::NetRequest {
   public:
    virtual ~ImgurAlbumCreation() = default;

    struct Result {
        QString deleteHash;
        QString id;
    };

    class Sink : public Net::Sink {
       public:
        Sink(std::shared_ptr<Result> res) : m_result(res) {};
        virtual ~Sink() = default;

       public:
        auto init(QNetworkRequest& request) -> Task::State override;
        auto write(QByteArray& data) -> Task::State override;
        auto abort() -> Task::State override;
        auto finalize(QNetworkReply& reply) -> Task::State override;
        auto hasLocalData() -> bool override { return false; }

       private:
        std::shared_ptr<Result> m_result;
        QByteArray m_output;
    };

    static NetRequest::Ptr make(std::shared_ptr<Result> output, QList<ScreenShot::Ptr> screenshots);
    QNetworkReply* getReply(QNetworkRequest& request) override;

   private:
    QList<ScreenShot::Ptr> m_screenshots;
};
