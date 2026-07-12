#include "AtlUserInteractionSupportImpl.h"
#include <QMessageBox>

#include "AtlOptionalModDialog.h"
#include "ui/dialogs/VersionSelectDialog.h"

AtlUserInteractionSupportImpl::AtlUserInteractionSupportImpl(QWidget* parent) : m_parent(parent) {}

std::optional<QList<QString>> AtlUserInteractionSupportImpl::chooseOptionalMods(const ATLauncher::PackVersion& version,
                                                                                QList<ATLauncher::VersionMod> mods)
{
    AtlOptionalModDialog optionalModDialog(m_parent, version, mods);
    auto result = optionalModDialog.exec();
    if (result == QDialog::Rejected) {
        return {};
    }
    return optionalModDialog.getResult();
}

QString AtlUserInteractionSupportImpl::chooseVersion(Meta::VersionList::Ptr vlist, QString minecraftVersion)
{
    VersionSelectDialog vselect(vlist.get(), "Choose Version", m_parent, false);
    if (minecraftVersion != nullptr) {
        vselect.setExactFilter(BaseVersionList::ParentVersionRole, minecraftVersion);
        vselect.setEmptyString(tr("No versions are currently available for Minecraft %1").arg(minecraftVersion));
    } else {
        vselect.setEmptyString(tr("No versions are currently available"));
    }
    vselect.setEmptyErrorString(tr("Couldn't load or download the version lists!"));

    // select recommended build
    for (int i = 0; i < vlist->versions().size(); i++) {
        auto version = vlist->versions().at(i);
        auto reqs = version->requiredSet();

        // filter by minecraft version, if the loader depends on a certain version.
        if (minecraftVersion != nullptr) {
            auto iter = std::find_if(reqs.begin(), reqs.end(), [](const Meta::Require& req) { return req.uid == "net.minecraft"; });
            if (iter == reqs.end())
                continue;
            if (iter->equalsVersion != minecraftVersion)
                continue;
        }

        // first recommended build we find, we use.
        if (version->isRecommended()) {
            vselect.setCurrentVersion(version->descriptor());
            break;
        }
    }

    vselect.exec();
    return vselect.selectedVersion()->descriptor();
}

void AtlUserInteractionSupportImpl::displayMessage(QString message)
{
    QMessageBox::information(m_parent, tr("Installing"), message);
}
