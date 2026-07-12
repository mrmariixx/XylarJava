#pragma once

#include <QDateTime>
#include <QJsonObject>
#include <QString>

#include <memory>

#include "BaseVersion.h"
#include "java/JavaVersion.h"

namespace Java {

enum class DownloadType { Manifest, Archive, Unknown };

class Metadata : public BaseVersion {
   public:
    virtual QString descriptor() const override { return version.toString(); }

    virtual QString name() const override { return m_name; }

    virtual QString typeString() const override { return vendor; }

    virtual bool operator<(BaseVersion& a) const override;
    virtual bool operator>(BaseVersion& a) const override;
    bool operator<(const Metadata& rhs) const;
    bool operator==(const Metadata& rhs) const;
    bool operator>(const Metadata& rhs) const;

    QString m_name;
    QString vendor;
    QString url;
    QDateTime releaseTime;
    QString checksumType;
    QString checksumHash;
    DownloadType downloadType;
    QString packageType;
    JavaVersion version;
    QString runtimeOS;
};
using MetadataPtr = std::shared_ptr<Metadata>;

DownloadType parseDownloadType(QString javaDownload);
QString downloadTypeToString(DownloadType javaDownload);
MetadataPtr parseJavaMeta(const QJsonObject& libObj);

}  // namespace Java
