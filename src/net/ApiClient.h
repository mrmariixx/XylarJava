#pragma once

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QObject>
#include <QUrl>

namespace xylar {

class ApiClient final : public QObject
{
    Q_OBJECT

public:
    explicit ApiClient(QObject *parent = nullptr);

    [[nodiscard]] QNetworkRequest requestFor(const QUrl &url) const;

private:
    QNetworkAccessManager m_network;
};

} // namespace xylar
