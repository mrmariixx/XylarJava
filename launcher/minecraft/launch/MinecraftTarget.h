#pragma once

#include <memory>

#include <QString>

struct MinecraftTarget {
    QString address;
    quint16 port;

    QString world;
    static MinecraftTarget parse(const QString& fullAddress, bool useWorld);
    using Ptr = std::shared_ptr<MinecraftTarget>;
};
