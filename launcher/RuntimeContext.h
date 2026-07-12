#pragma once

#include <QSet>
#include <QString>
#include "SysInfo.h"
#include "settings/SettingsObject.h"

struct RuntimeContext {
    QString javaArchitecture;
    QString javaRealArchitecture;
    QString system = SysInfo::currentSystem();

    QString mappedJavaRealArchitecture() const
    {
        if (javaRealArchitecture == "amd64")
            return "x86_64";
        if (javaRealArchitecture == "i386" || javaRealArchitecture == "i686")
            return "x86";
        if (javaRealArchitecture == "aarch64")
            return "arm64";
        if (javaRealArchitecture == "arm" || javaRealArchitecture == "armhf")
            return "arm32";
        return javaRealArchitecture;
    }

    void updateFromInstanceSettings(SettingsObject* instanceSettings)
    {
        javaArchitecture = instanceSettings->get("JavaArchitecture").toString();
        javaRealArchitecture = instanceSettings->get("JavaRealArchitecture").toString();
    }

    QString getClassifier() const { return system + "-" + mappedJavaRealArchitecture(); }

    // "Legacy" refers to the fact that Mojang assumed that these are the only two architectures
    bool isLegacyArch() const
    {
        const QString mapped = mappedJavaRealArchitecture();
        return mapped == "x86_64" || mapped == "x86";
    }

    bool classifierMatches(QString target) const
    {
        // try to match precise classifier "[os]-[arch]"
        bool x = target == getClassifier();
        // try to match imprecise classifier on legacy architectures "[os]"
        if (!x && isLegacyArch())
            x = target == system;

        return x;
    }
};
