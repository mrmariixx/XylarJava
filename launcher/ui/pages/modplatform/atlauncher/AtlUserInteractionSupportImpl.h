#pragma once

#include <QObject>

#include "modplatform/atlauncher/ATLPackInstallTask.h"

class AtlUserInteractionSupportImpl : public QObject, public ATLauncher::UserInteractionSupport {
    Q_OBJECT

   public:
    AtlUserInteractionSupportImpl(QWidget* parent);
    virtual ~AtlUserInteractionSupportImpl() = default;

   private:
    QString chooseVersion(Meta::VersionList::Ptr vlist, QString minecraftVersion) override;
    std::optional<QList<QString>> chooseOptionalMods(const ATLauncher::PackVersion& version, QList<ATLauncher::VersionMod> mods) override;
    void displayMessage(QString message) override;

   private:
    QWidget* m_parent;
};
