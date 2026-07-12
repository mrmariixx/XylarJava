#include "java/JavaMetadata.h"

#include <memory>

#include "Json.h"
#include "StringUtils.h"
#include "java/JavaVersion.h"
#include "minecraft/ParseUtils.h"

namespace Java {

DownloadType parseDownloadType(QString javaDownload)
{
    if (javaDownload == "manifest")
        return DownloadType::Manifest;
    else if (javaDownload == "archive")
        return DownloadType::Archive;
    else
        return DownloadType::Unknown;
}
QString downloadTypeToString(DownloadType javaDownload)
{
    switch (javaDownload) {
        case DownloadType::Manifest:
            return "manifest";
        case DownloadType::Archive:
            return "archive";
        case DownloadType::Unknown:
            break;
    }
    return "unknown";
}
MetadataPtr parseJavaMeta(const QJsonObject& in)
{
    auto meta = std::make_shared<Metadata>();

    meta->m_name = in["name"].toString("");
    meta->vendor = in["vendor"].toString("");
    meta->url = in["url"].toString("");
    meta->releaseTime = timeFromS3Time(in["releaseTime"].toString(""));
    meta->downloadType = parseDownloadType(in["downloadType"].toString(""));
    meta->packageType = in["packageType"].toString("");
    meta->runtimeOS = in["runtimeOS"].toString("unknown");

    if (in.contains("checksum")) {
        auto obj = Json::requireObject(in, "checksum");
        meta->checksumHash = obj["hash"].toString("");
        meta->checksumType = obj["type"].toString("");
    }

    if (in.contains("version")) {
        auto obj = Json::requireObject(in, "version");
        auto name = obj["name"].toString("");
        auto major = obj["major"].toInteger();
        auto minor = obj["minor"].toInteger();
        auto security = obj["security"].toInteger();
        auto build = obj["build"].toInteger();
        meta->version = JavaVersion(major, minor, security, build, name);
    }
    return meta;
}

bool Metadata::operator<(const Metadata& rhs) const
{
    auto id = version;
    if (id < rhs.version) {
        return true;
    }
    if (id > rhs.version) {
        return false;
    }
    auto date = releaseTime;
    if (date < rhs.releaseTime) {
        return true;
    }
    if (date > rhs.releaseTime) {
        return false;
    }
    return StringUtils::naturalCompare(m_name, rhs.m_name, Qt::CaseInsensitive) < 0;
}

bool Metadata::operator==(const Metadata& rhs) const
{
    return version == rhs.version && m_name == rhs.m_name;
}

bool Metadata::operator>(const Metadata& rhs) const
{
    return (!operator<(rhs)) && (!operator==(rhs));
}

bool Metadata::operator<(BaseVersion& a) const
{
    try {
        return operator<(dynamic_cast<Metadata&>(a));
    } catch (const std::bad_cast&) {
        return BaseVersion::operator<(a);
    }
}

bool Metadata::operator>(BaseVersion& a) const
{
    try {
        return operator>(dynamic_cast<Metadata&>(a));
    } catch (const std::bad_cast&) {
        return BaseVersion::operator>(a);
    }
}

}  // namespace Java
