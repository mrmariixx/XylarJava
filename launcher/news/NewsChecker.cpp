#include "NewsChecker.h"

#include <QByteArray>
#include <QDomDocument>

#include <QDebug>
#include "Application.h"

NewsChecker::NewsChecker(QNetworkAccessManager* network, const QString& feedUrl)
{
    m_network = network;
    m_feedUrl = feedUrl;
}

void NewsChecker::reloadNews()
{
    // Start a netjob to download the RSS feed and call rssDownloadFinished() when it's done.
    if (isLoadingNews()) {
        qDebug() << "Ignored request to reload news. Currently reloading already.";
        return;
    }

    m_entry = APPLICATION->metacache()->resolveEntry("feed", "feed.xml");

    qDebug() << "Reloading news.";

    NetJob::Ptr job{ new NetJob("News RSS Feed", m_network) };
    job->addNetAction(Net::Download::makeCached(m_feedUrl, m_entry));
    job->setAskRetry(false);
    connect(job.get(), &NetJob::succeeded, this, &NewsChecker::rssDownloadFinished);
    connect(job.get(), &NetJob::failed, this, &NewsChecker::rssDownloadFailed);
    m_newsNetJob.reset(job);
    job->start();
}

void NewsChecker::rssDownloadFinished()
{
    // Parse the XML file and process the RSS feed entries.
    qDebug() << "Finished loading RSS feed.";

    m_newsNetJob.reset();
    QDomDocument doc;
    {
        // Stuff to store error info in.
        QString errorMsg = "Unknown error.";
        int errorLine = -1;
        int errorCol = -1;

        QFile feed(m_entry->getFullPath());

        if (feed.open(QFile::ReadOnly | QFile::Text)) {
            QTextStream in(&feed);
            // Parse the XML.
            if (!doc.setContent(in.readAll(), false, &errorMsg, &errorLine, &errorCol)) {
                fail(QString("Error parsing RSS feed XML. %1 at %2:%3.").arg(errorMsg).arg(errorLine).arg(errorCol));
                return;
            }
        }
    }

    // If the parsing succeeded, read it.
    QDomNodeList items = doc.elementsByTagName("entry");
    m_newsEntries.clear();
    for (int i = 0; i < items.length(); i++) {
        QDomElement element = items.at(i).toElement();
        NewsEntryPtr entry;
        entry.reset(new NewsEntry());
        QString errorMsg = "An unknown error occurred.";
        if (NewsEntry::fromXmlElement(element, entry.get(), &errorMsg)) {
            qDebug() << "Loaded news entry" << entry->title;
            m_newsEntries.append(entry);
        } else {
            qWarning() << "Failed to load news entry at index" << i << ":" << errorMsg;
        }
    }

    succeed();
}

void NewsChecker::rssDownloadFailed(QString reason)
{
    // Set an error message and fail.
    fail(tr("Failed to load news RSS feed:\n%1").arg(reason));
}

QList<NewsEntryPtr> NewsChecker::getNewsEntries() const
{
    return m_newsEntries;
}

bool NewsChecker::isLoadingNews() const
{
    return m_newsNetJob.get() != nullptr;
}

QString NewsChecker::getLastLoadErrorMsg() const
{
    return m_lastLoadError;
}

void NewsChecker::succeed()
{
    m_lastLoadError = "";
    qDebug() << "News loading succeeded.";
    m_newsNetJob.reset();
    emit newsLoaded();
}

void NewsChecker::fail(const QString& errorMsg)
{
    m_lastLoadError = errorMsg;
    qDebug() << "Failed to load news:" << errorMsg;
    m_newsNetJob.reset();
    emit newsLoadingFailed(errorMsg);
}
