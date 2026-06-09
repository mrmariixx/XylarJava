#include "download/DownloadManager.h"

#include <QCryptographicHash>
#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QNetworkReply>
#include <QSaveFile>

namespace xylar {

DownloadManager::DownloadManager(QObject *parent)
    : QObject(parent)
{
}

void DownloadManager::enqueue(const QUrl &url)
{
    m_queue.append(url);
    emit queuedCountChanged(m_queue.size());
}

void DownloadManager::beginBatch(int totalCount)
{
    m_totalCount = totalCount;
    m_completedCount = 0;
    emit progressChanged(0, m_totalCount, QStringLiteral("Ready"));
}

int DownloadManager::queuedCount() const
{
    return m_queue.size();
}

bool DownloadManager::downloadToFile(const DownloadRequest &request, QString *errorMessage)
{
    if (!request.url.isValid() || request.targetPath.isEmpty()) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Invalid download request for %1").arg(request.label);
        }
        return false;
    }

    QDir().mkpath(QFileInfo(request.targetPath).absolutePath());
    if (!request.force && isFileValid(request)) {
        ++m_completedCount;
        emit progressChanged(m_completedCount, m_totalCount, request.label);
        emit logLine(QStringLiteral("OK %1").arg(request.label));
        return true;
    }

    emit logLine(QStringLiteral("Downloading %1").arg(request.label.isEmpty() ? request.url.toString() : request.label));

    QNetworkRequest networkRequest(request.url);
    networkRequest.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("XylarJava/%1").arg(QStringLiteral(XYLARJAVA_VERSION)));
    networkRequest.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    QNetworkReply *reply = m_network.get(networkRequest);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() != QNetworkReply::NoError) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("%1: %2").arg(request.label, reply->errorString());
        }
        reply->deleteLater();
        return false;
    }

    const int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (httpStatus >= 400) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("%1: HTTP %2").arg(request.label).arg(httpStatus);
        }
        reply->deleteLater();
        return false;
    }

    const QByteArray payload = reply->readAll();
    reply->deleteLater();

    if (request.expectedSize > 0 && payload.size() != request.expectedSize) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("%1: expected %2 bytes, got %3")
                .arg(request.label)
                .arg(request.expectedSize)
                .arg(payload.size());
        }
        return false;
    }

    if (!request.sha1.isEmpty()) {
        const QString hash = QString::fromLatin1(QCryptographicHash::hash(payload, QCryptographicHash::Sha1).toHex());
        if (hash.compare(request.sha1, Qt::CaseInsensitive) != 0) {
            if (errorMessage) {
                *errorMessage = QStringLiteral("%1: SHA1 mismatch").arg(request.label);
            }
            return false;
        }
    }
    if (!request.sha512.isEmpty()) {
        const QString hash = QString::fromLatin1(QCryptographicHash::hash(payload, QCryptographicHash::Sha512).toHex());
        if (hash.compare(request.sha512, Qt::CaseInsensitive) != 0) {
            if (errorMessage) {
                *errorMessage = QStringLiteral("%1: SHA512 mismatch").arg(request.label);
            }
            return false;
        }
    }

    QSaveFile file(request.targetPath);
    if (!file.open(QIODevice::WriteOnly)) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("%1: cannot write %2").arg(request.label, request.targetPath);
        }
        return false;
    }
    file.write(payload);
    if (!file.commit()) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("%1: cannot commit %2").arg(request.label, request.targetPath);
        }
        return false;
    }

    ++m_completedCount;
    emit progressChanged(m_completedCount, m_totalCount, request.label);
    emit logLine(QStringLiteral("Saved %1").arg(request.label));
    return true;
}

int DownloadManager::completedCount() const
{
    return m_completedCount;
}

bool DownloadManager::isFileValid(const DownloadRequest &request) const
{
    const QFileInfo file(request.targetPath);
    if (!file.exists() || !file.isFile()) {
        return false;
    }

    if (request.expectedSize > 0 && file.size() != request.expectedSize) {
        return false;
    }

    if (!request.sha1.isEmpty() && sha1ForFile(request.targetPath).compare(request.sha1, Qt::CaseInsensitive) != 0) {
        return false;
    }
    if (!request.sha512.isEmpty() && sha512ForFile(request.targetPath).compare(request.sha512, Qt::CaseInsensitive) != 0) {
        return false;
    }

    return true;
}

QString DownloadManager::sha1ForFile(const QString &path) const
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return {};
    }

    QCryptographicHash hash(QCryptographicHash::Sha1);
    while (!file.atEnd()) {
        hash.addData(file.read(1024 * 1024));
    }
    return QString::fromLatin1(hash.result().toHex());
}

QString DownloadManager::sha512ForFile(const QString &path) const
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return {};
    }

    QCryptographicHash hash(QCryptographicHash::Sha512);
    while (!file.atEnd()) {
        hash.addData(file.read(1024 * 1024));
    }
    return QString::fromLatin1(hash.result().toHex());
}

} // namespace xylar
