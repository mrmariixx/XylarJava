#pragma once

#include "BaseVersion.h"
#include "JavaVersion.h"

struct JavaInstall : public BaseVersion {
    JavaInstall() {}
    JavaInstall(QString id, QString arch, QString path) : id(id), arch(arch), path(path) {}
    virtual QString descriptor() const override { return id.toString(); }

    virtual QString name() const override { return id.toString(); }

    virtual QString typeString() const override { return arch; }

    virtual bool operator<(BaseVersion& a) const override;
    virtual bool operator>(BaseVersion& a) const override;
    bool operator<(const JavaInstall& rhs) const;
    bool operator==(const JavaInstall& rhs) const;
    bool operator>(const JavaInstall& rhs) const;

    JavaVersion id;
    QString arch;
    QString path;
    bool is_64bit = false;
};

using JavaInstallPtr = std::shared_ptr<JavaInstall>;
