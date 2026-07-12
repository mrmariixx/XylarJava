#pragma once

#include <QFileInfo>
#include "Screenshot.h"
#include "net/NetRequest.h"

class ImgurUpload : public Net::NetRequest {
   public:
    class Sink : public Net::Sink {
       public:
        Sink(ScreenShot::Ptr shot) : m_shot(shot) {};
        virtual ~Sink() = default;

       public:
        auto init(QNetworkRequest& request) -> Task::State override;
        auto write(QByteArray& data) -> Task::State override;
        auto abort() -> Task::State override;
        auto finalize(QNetworkReply& reply) -> Task::State override;
        auto hasLocalData() -> bool override { return false; }

       private:
        ScreenShot::Ptr m_shot;
        QByteArray m_output;
    };
    ImgurUpload(QFileInfo info) : m_fileInfo(info) {}
    virtual ~ImgurUpload() = default;

    static NetRequest::Ptr make(ScreenShot::Ptr m_shot);

   private:
    virtual QNetworkReply* getReply(QNetworkRequest&) override;
    const QFileInfo m_fileInfo;
};
