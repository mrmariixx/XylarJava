#include "FTBPackManifest.h"

#include "Json.h"

static void loadSpecs(FTB::Specs& s, QJsonObject& obj)
{
    s.id = Json::requireInteger(obj, "id");
    s.minimum = Json::requireInteger(obj, "minimum");
    s.recommended = Json::requireInteger(obj, "recommended");
}

static void loadTag(FTB::Tag& t, QJsonObject& obj)
{
    t.id = Json::requireInteger(obj, "id");
    t.name = Json::requireString(obj, "name");
}

static void loadArt(FTB::Art& a, QJsonObject& obj)
{
    a.id = Json::requireInteger(obj, "id");
    a.url = Json::requireString(obj, "url");
    a.type = Json::requireString(obj, "type");
    a.width = Json::requireInteger(obj, "width");
    a.height = Json::requireInteger(obj, "height");
    a.compressed = Json::requireBoolean(obj, "compressed");
    a.sha1 = Json::requireString(obj, "sha1");
    a.size = obj["size"].toInt();
    a.updated = Json::requireInteger(obj, "updated");
}

static void loadAuthor(FTB::Author& a, QJsonObject& obj)
{
    a.id = Json::requireInteger(obj, "id");
    a.name = Json::requireString(obj, "name");
    a.type = Json::requireString(obj, "type");
    a.website = Json::requireString(obj, "website");
    a.updated = Json::requireInteger(obj, "updated");
}

static void loadVersionInfo(FTB::VersionInfo& v, QJsonObject& obj)
{
    v.id = Json::requireInteger(obj, "id");
    v.name = Json::requireString(obj, "name");
    v.type = Json::requireString(obj, "type");
    v.updated = Json::requireInteger(obj, "updated");
    auto specs = Json::requireObject(obj, "specs");
    loadSpecs(v.specs, specs);
}

void FTB::loadModpack(FTB::Modpack& m, QJsonObject& obj)
{
    m.id = Json::requireInteger(obj, "id");
    m.name = Json::requireString(obj, "name");
    m.safeName = Json::requireString(obj, "name").replace(QRegularExpression("[^A-Za-z0-9]"), "").toLower() + ".png";
    m.synopsis = Json::requireString(obj, "synopsis");
    m.description = Json::requireString(obj, "description");
    m.type = Json::requireString(obj, "type");
    m.featured = Json::requireBoolean(obj, "featured");
    m.installs = Json::requireInteger(obj, "installs");
    m.plays = Json::requireInteger(obj, "plays");
    m.updated = Json::requireInteger(obj, "updated");
    m.refreshed = obj["refreshed"].toInt();
    auto artArr = Json::requireArray(obj, "art");
    for (QJsonValueRef artRaw : artArr) {
        auto artObj = Json::requireObject(artRaw);
        FTB::Art art;
        loadArt(art, artObj);
        m.art.append(art);
    }
    auto authorArr = Json::requireArray(obj, "authors");
    for (QJsonValueRef authorRaw : authorArr) {
        auto authorObj = Json::requireObject(authorRaw);
        FTB::Author author;
        loadAuthor(author, authorObj);
        m.authors.append(author);
    }
    auto versionArr = Json::requireArray(obj, "versions");
    for (QJsonValueRef versionRaw : versionArr) {
        auto versionObj = Json::requireObject(versionRaw);
        FTB::VersionInfo version;
        loadVersionInfo(version, versionObj);
        m.versions.append(version);
    }
    auto tagArr = Json::requireArray(obj, "tags");
    for (QJsonValueRef tagRaw : tagArr) {
        auto tagObj = Json::requireObject(tagRaw);
        FTB::Tag tag;
        loadTag(tag, tagObj);
        m.tags.append(tag);
    }
    m.updated = Json::requireInteger(obj, "updated");
}

static void loadVersionTarget(FTB::VersionTarget& a, QJsonObject& obj)
{
    a.id = Json::requireInteger(obj, "id");
    a.name = Json::requireString(obj, "name");
    a.type = Json::requireString(obj, "type");
    a.version = Json::requireString(obj, "version");
    a.updated = Json::requireInteger(obj, "updated");
}

static void loadVersionFile(FTB::VersionFile& a, QJsonObject& obj)
{
    a.id = Json::requireInteger(obj, "id");
    a.type = Json::requireString(obj, "type");
    a.path = Json::requireString(obj, "path");
    a.name = Json::requireString(obj, "name");
    a.version = Json::requireString(obj, "version");
    a.url = obj["url"].toString();  // optional
    a.sha1 = Json::requireString(obj, "sha1");
    a.size = obj["size"].toInt();
    a.clientOnly = Json::requireBoolean(obj, "clientonly");
    a.serverOnly = Json::requireBoolean(obj, "serveronly");
    a.optional = Json::requireBoolean(obj, "optional");
    a.updated = Json::requireInteger(obj, "updated");
    auto curseforgeObj = obj["curseforge"].toObject();  // optional
    a.curseforge.project_id = curseforgeObj["project"].toInt();
    a.curseforge.file_id = curseforgeObj["file"].toInt();
}

void FTB::loadVersion(FTB::Version& m, QJsonObject& obj)
{
    m.id = Json::requireInteger(obj, "id");
    m.parent = Json::requireInteger(obj, "parent");
    m.name = Json::requireString(obj, "name");
    m.type = Json::requireString(obj, "type");
    m.installs = Json::requireInteger(obj, "installs");
    m.plays = Json::requireInteger(obj, "plays");
    m.updated = Json::requireInteger(obj, "updated");
    m.refreshed = obj["refreshed"].toInt();
    auto specs = Json::requireObject(obj, "specs");
    loadSpecs(m.specs, specs);
    auto targetArr = Json::requireArray(obj, "targets");
    for (QJsonValueRef targetRaw : targetArr) {
        auto versionObj = Json::requireObject(targetRaw);
        FTB::VersionTarget target;
        loadVersionTarget(target, versionObj);
        m.targets.append(target);
    }
    auto fileArr = Json::requireArray(obj, "files");
    for (QJsonValueRef fileRaw : fileArr) {
        auto fileObj = Json::requireObject(fileRaw);
        FTB::VersionFile file;
        loadVersionFile(file, fileObj);
        m.files.append(file);
    }
}
