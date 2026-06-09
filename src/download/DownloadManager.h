#pragma once

#include <QFileInfo>
#include <QList>
#include <QNetworkAccessManager>
#include <QObject>
#include <QString>
#include <QUrl>
#include <functional>

namespace xylar {

struct DownloadRequest
{
    QUrl url;
    QString targetPath;
    QString sha1;
    QString sha512;
    qint64 expectedSize = -1;
    QString label;
    bool force = false;
};

class DownloadManager final : public QObject
{
    Q_OBJECT

public:
    explicit DownloadManager(QObject *parent = nullptr);

    void enqueue(const QUrl &url);
    void beginBatch(int totalCount);
    [[nodiscard]] int queuedCount() const;
    [[nodiscard]] bool downloadToFile(const DownloadRequest &request, QString *errorMessage = nullptr);
    [[nodiscard]] int completedCount() const;

signals:
    void queuedCountChanged(int count);
    void progressChanged(int completed, int total, const QString &label);
    void logLine(const QString &line);

private:
    [[nodiscard]] bool isFileValid(const DownloadRequest &request) const;
    [[nodiscard]] QString sha1ForFile(const QString &path) const;
    [[nodiscard]] QString sha512ForFile(const QString &path) const;

    QNetworkAccessManager m_network;
    QList<QUrl> m_queue;
    int m_completedCount = 0;
    int m_totalCount = 0;
};

} // namespace xylar
