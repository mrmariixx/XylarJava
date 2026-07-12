#include "MetaCacheSink.h"
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include "Application.h"

#include "net/Logging.h"

namespace Net {

/** Maximum time to hold a cache entry
 *  = 1 week in seconds
 */
#define MAX_TIME_TO_EXPIRE 1 * 7 * 24 * 60 * 60

MetaCacheSink::MetaCacheSink(MetaEntryPtr entry, ChecksumValidator* md5sum, bool is_eternal)
    : Net::FileSink(entry->getFullPath()), m_entry(entry), m_md5Node(md5sum), m_is_eternal(is_eternal)
{
    addValidator(md5sum);
}

Task::State MetaCacheSink::initCache(QNetworkRequest& request)
{
    if (!m_entry->isStale()) {
        return Task::State::Succeeded;
    }

    // check if file exists, if it does, use its information for the request
    QFile current(m_filename);
    if (current.exists() && current.size() != 0) {
        if (m_entry->getRemoteChangedTimestamp().size()) {
            request.setRawHeader(QString("If-Modified-Since").toLatin1(), m_entry->getRemoteChangedTimestamp().toLatin1());
        }
        if (m_entry->getETag().size()) {
            request.setRawHeader(QString("If-None-Match").toLatin1(), m_entry->getETag().toLatin1());
        }
    }

    return Task::State::Running;
}

Task::State MetaCacheSink::finalizeCache(QNetworkReply& reply)
{
    QFileInfo output_file_info(m_filename);

    if (m_wroteAnyData) {
        m_entry->setMD5Sum(m_md5Node->hash().toHex().constData());
    }

    m_entry->setETag(reply.rawHeader("ETag").constData());

    if (reply.hasRawHeader("Last-Modified")) {
        m_entry->setRemoteChangedTimestamp(reply.rawHeader("Last-Modified").constData());
    }

    m_entry->setLocalChangedTimestamp(output_file_info.lastModified().toUTC().toMSecsSinceEpoch());

    {  // Cache lifetime
        if (m_is_eternal) {
            qCDebug(taskMetaCacheLogC) << "Adding eternal cache entry:" << m_entry->getFullPath();
            m_entry->makeEternal(true);
        } else if (reply.hasRawHeader("Cache-Control")) {
            auto cache_control_header = reply.rawHeader("Cache-Control");
            qCDebug(taskMetaCacheLogC) << "Parsing 'Cache-Control' header with" << cache_control_header;

            static const QRegularExpression s_maxAgeExpr("max-age=([0-9]+)");
            qint64 max_age = s_maxAgeExpr.match(cache_control_header).captured(1).toLongLong();
            m_entry->setMaximumAge(max_age);

        } else if (reply.hasRawHeader("Expires")) {
            auto expires_header = reply.rawHeader("Expires");
            qCDebug(taskMetaCacheLogC) << "Parsing 'Expires' header with" << expires_header;

            qint64 max_age = QDateTime::fromString(expires_header).toSecsSinceEpoch() - QDateTime::currentSecsSinceEpoch();
            m_entry->setMaximumAge(max_age);
        } else {
            m_entry->setMaximumAge(MAX_TIME_TO_EXPIRE);
        }

        if (reply.hasRawHeader("Age")) {
            auto age_header = reply.rawHeader("Age");
            qCDebug(taskMetaCacheLogC) << "Parsing 'Age' header with" << age_header;

            qint64 current_age = age_header.toLongLong();
            m_entry->setCurrentAge(current_age);
        } else {
            m_entry->setCurrentAge(0);
        }
    }

    m_entry->setStale(false);
    APPLICATION->metacache()->updateEntry(m_entry);

    return Task::State::Succeeded;
}

bool MetaCacheSink::hasLocalData()
{
    QFileInfo info(m_filename);
    return info.exists() && info.size() != 0;
}
}  // namespace Net
