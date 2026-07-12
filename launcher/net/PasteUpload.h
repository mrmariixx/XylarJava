#pragma once

#include "net/ByteArraySink.h"
#include "net/NetRequest.h"
#include "tasks/Task.h"

#include <QNetworkReply>
#include <QRegularExpression>
#include <QString>

#include <array>
#include <memory>
#include <utility>

class PasteUpload : public Net::NetRequest {
   public:
    enum PasteType : int {
        // 0x0.st
        NullPointer,
        // hastebin.com
        Hastebin,
        // paste.gg
        PasteGG,
        // mclo.gs
        Mclogs,
        // Helpful to get the range of valid values on the enum for input sanitisation:
        First = NullPointer,
        Last = Mclogs
    };
    struct PasteTypeInfo {
        const QString name;
        const QString defaultBase;
        const QString endpointPath;
    };

    static const std::array<PasteTypeInfo, 4> PasteTypes;

    class Sink : public Net::ByteArraySink {
       public:
        Sink(PasteUpload* p) : m_d(p) {};
        virtual ~Sink() = default;

       public:
        auto finalize(QNetworkReply& reply) -> Task::State override;

       private:
        PasteUpload* m_d;
    };
    friend Sink;

    PasteUpload(const QString& log, QString url, PasteType pasteType);
    virtual ~PasteUpload() = default;

    QString pasteLink() { return m_pasteLink; }

   private:
    virtual QNetworkReply* getReply(QNetworkRequest&) override;
    QString m_log;
    QString m_pasteLink;
    QString m_baseUrl;
    const PasteType m_paste_type;
};
