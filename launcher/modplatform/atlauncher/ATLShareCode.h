#pragma once

#include <QJsonObject>
#include <QList>
#include <QString>

namespace ATLauncher {

struct ShareCodeMod {
    bool selected;
    QString name;
};

struct ShareCode {
    QString pack;
    QString version;
    QList<ShareCodeMod> mods;
};

struct ShareCodeResponse {
    bool error;
    int code;
    QString message;
    ShareCode data;
};

void loadShareCodeResponse(ShareCodeResponse& r, QJsonObject& obj);

}  // namespace ATLauncher
