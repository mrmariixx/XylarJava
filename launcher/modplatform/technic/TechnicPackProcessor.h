#pragma once

#include <QString>
#include "settings/SettingsObject.h"

namespace Technic {
// not exporting it, only used in SingleZipPackInstallTask, InstanceImportTask and SolderPackInstallTask
class TechnicPackProcessor : public QObject {
    Q_OBJECT

   signals:
    void succeeded();
    void failed(QString reason);

   public:
    void run(SettingsObject* globalSettings,
             const QString& instName,
             const QString& instIcon,
             const QString& stagingPath,
             const QString& minecraftVersion = QString(),
             bool isSolder = false);
};
}  // namespace Technic
