#pragma once

#include <QJsonObject>

#include <set>
#include "Exception.h"

namespace Meta {
class Index;
class Version;
class VersionList;

enum class MetadataVersion { Invalid = -1, InitialRelease = 1 };

class ParseException : public Exception {
   public:
    using Exception::Exception;
};
struct Require {
    bool operator==(const Require& rhs) const { return uid == rhs.uid; }
    bool operator<(const Require& rhs) const { return uid < rhs.uid; }
    bool deepEquals(const Require& rhs) const { return uid == rhs.uid && equalsVersion == rhs.equalsVersion && suggests == rhs.suggests; }
    QString uid;
    QString equalsVersion;
    QString suggests;
};

using RequireSet = std::set<Require>;

void parseIndex(const QJsonObject& obj, Index* ptr);
void parseVersion(const QJsonObject& obj, Version* ptr);
void parseVersionList(const QJsonObject& obj, VersionList* ptr);

MetadataVersion parseFormatVersion(const QJsonObject& obj, bool required = true);
void serializeFormatVersion(QJsonObject& obj, MetadataVersion version);

// FIXME: this has a different shape than the others...FIX IT!?
void parseRequires(const QJsonObject& obj, RequireSet* ptr, const char* keyName = "requires");
void serializeRequires(QJsonObject& objOut, RequireSet* ptr, const char* keyName = "requires");
MetadataVersion currentFormatVersion();
}  // namespace Meta

Q_DECLARE_METATYPE(std::set<Meta::Require>)
