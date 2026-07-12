#pragma once
#include <QList>
#include <QString>
#include "minecraft/mod/Mod.h"

namespace ExportToModList {

enum Formats { HTML, MARKDOWN, PLAINTXT, JSON, CSV, CUSTOM };
enum OptionalData { Authors = 1 << 0, Url = 1 << 1, Version = 1 << 2, FileName = 1 << 3 };
QString exportToModList(QList<Mod*> mods, Formats format, OptionalData extraData);
QString exportToModList(QList<Mod*> mods, QString lineTemplate);
}  // namespace ExportToModList
