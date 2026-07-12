#include "SolderPackManifest.h"

#include "Json.h"

namespace TechnicSolder {

void loadPack(Pack& v, QJsonObject& obj)
{
    v.recommended = Json::requireString(obj, "recommended");
    v.latest = Json::requireString(obj, "latest");

    auto builds = Json::requireArray(obj, "builds");
    for (const auto buildRaw : builds) {
        auto build = Json::requireString(buildRaw);
        v.builds.append(build);
    }
}

static void loadPackBuildMod(PackBuildMod& b, QJsonObject& obj)
{
    b.name = Json::requireString(obj, "name");
    b.version = obj["version"].toString("");
    b.md5 = Json::requireString(obj, "md5");
    b.url = Json::requireString(obj, "url");
}

void loadPackBuild(PackBuild& v, QJsonObject& obj)
{
    v.minecraft = Json::requireString(obj, "minecraft");

    auto mods = Json::requireArray(obj, "mods");
    for (const auto modRaw : mods) {
        auto modObj = Json::requireObject(modRaw);
        PackBuildMod mod;
        loadPackBuildMod(mod, modObj);
        v.mods.append(mod);
    }
}

}  // namespace TechnicSolder
