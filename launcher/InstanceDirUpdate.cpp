#include "InstanceDirUpdate.h"

#include <QCheckBox>

#include "Application.h"
#include "FileSystem.h"

#include "InstanceList.h"
#include "ui/dialogs/CustomMessageBox.h"

QString askToUpdateInstanceDirName(BaseInstance* instance, const QString& oldName, const QString& newName, QWidget* parent)
{
    if (oldName == newName)
        return QString();

    QString renamingMode = APPLICATION->settings()->get("InstRenamingMode").toString();
    if (renamingMode == "MetadataOnly")
        return QString();

    auto oldRoot = instance->instanceRoot();
    auto newDirName = FS::DirNameFromString(newName, QFileInfo(oldRoot).dir().absolutePath());
    auto newRoot = FS::PathCombine(QFileInfo(oldRoot).dir().absolutePath(), newDirName);
    if (oldRoot == newRoot)
        return QString();
    if (oldRoot == FS::PathCombine(QFileInfo(oldRoot).dir().absolutePath(), newName))
        return QString();

    // Check for conflict
    if (QDir(newRoot).exists()) {
        QMessageBox::warning(parent, QObject::tr("Cannot rename instance"),
                             QObject::tr("New instance root (%1) already exists. <br />Only the metadata will be renamed.").arg(newRoot));
        return QString();
    }

    // Ask if we should rename
    if (renamingMode == "AskEverytime") {
        auto checkBox = new QCheckBox(QObject::tr("&Remember my choice"), parent);
        auto dialog =
            CustomMessageBox::selectable(parent, QObject::tr("Rename instance folder"),
                                         QObject::tr("Would you also like to rename the instance folder?\n\n"
                                                     "Old name: %1\n"
                                                     "New name: %2")
                                             .arg(oldName, newName),
                                         QMessageBox::Question, QMessageBox::No | QMessageBox::Yes, QMessageBox::NoButton, checkBox);

        auto res = dialog->exec();
        if (checkBox->isChecked()) {
            if (res == QMessageBox::Yes)
                APPLICATION->settings()->set("InstRenamingMode", "PhysicalDir");
            else
                APPLICATION->settings()->set("InstRenamingMode", "MetadataOnly");
        }
        if (res == QMessageBox::No)
            return QString();
    }

    // Check for linked instances
    if (!checkLinkedInstances(instance->id(), parent, QObject::tr("Renaming")))
        return QString();

    // Now we can confirm that a renaming is happening
    if (!instance->syncInstanceDirName(newRoot)) {
        QMessageBox::warning(parent, QObject::tr("Cannot rename instance"),
                             QObject::tr("An error occurred when performing the following renaming operation: <br/>"
                                         " - Old instance root: %1<br/>"
                                         " - New instance root: %2<br/>"
                                         "Only the metadata is renamed.")
                                 .arg(oldRoot, newRoot));
        return QString();
    }
    return newRoot;
}

bool checkLinkedInstances(const QString& id, QWidget* parent, const QString& verb)
{
    auto linkedInstances = APPLICATION->instances()->getLinkedInstancesById(id);
    if (!linkedInstances.empty()) {
        auto response = CustomMessageBox::selectable(parent, QObject::tr("There are linked instances"),
                                                     QObject::tr("The following instance(s) might reference files in this instance:\n\n"
                                                                 "%1\n\n"
                                                                 "%2 it could break the other instance(s), \n\n"
                                                                 "Do you wish to proceed?",
                                                                 nullptr, linkedInstances.count())
                                                         .arg(linkedInstances.join("\n"))
                                                         .arg(verb),
                                                     QMessageBox::Warning, QMessageBox::Yes | QMessageBox::No, QMessageBox::No)
                            ->exec();
        if (response != QMessageBox::Yes)
            return false;
    }
    return true;
}
