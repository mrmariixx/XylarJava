#pragma once

#include <meta/VersionList.h>
#include "ATLPackManifest.h"

#include "InstanceTask.h"
#include "meta/Version.h"
#include "minecraft/MinecraftInstance.h"
#include "minecraft/PackProfile.h"
#include "net/NetJob.h"
#include "settings/INISettingsObject.h"

#include <memory>
#include <optional>

namespace ATLauncher {

enum class InstallMode {
    Install,
    Reinstall,
    Update,
};

class UserInteractionSupport {
   public:
    /**
     * Requests a user interaction to select which optional mods should be installed.
     */
    virtual std::optional<QList<QString>> chooseOptionalMods(const PackVersion& version, QList<ATLauncher::VersionMod> mods) = 0;

    /**
     * Requests a user interaction to select a component version from a given version list
     * and constrained to a given Minecraft version.
     */
    virtual QString chooseVersion(Meta::VersionList::Ptr vlist, QString minecraftVersion) = 0;

    /**
     * Requests a user interaction to display a message.
     */
    virtual void displayMessage(QString message) = 0;

    virtual ~UserInteractionSupport() = default;
};

class PackInstallTask : public InstanceTask {
    Q_OBJECT

   public:
    explicit PackInstallTask(UserInteractionSupport* support,
                             QString packName,
                             QString version,
                             InstallMode installMode = InstallMode::Install);
    virtual ~PackInstallTask() { delete m_support; }

    bool canAbort() const override { return true; }
    bool abort() override;

   protected:
    virtual void executeTask() override;

   private slots:
    void onDownloadSucceeded(QByteArray* responsePtr);
    void onDownloadFailed(QString reason);
    void onDownloadAborted();

    void onModsDownloaded();
    void onModsExtracted();

   private:
    QString getDirForModType(ModType type, QString raw);
    QString getVersionForLoader(QString uid);
    QString detectLibrary(const VersionLibrary& library);

    bool createLibrariesComponent(QString instanceRoot, PackProfile* profile);
    bool createPackComponent(QString instanceRoot, PackProfile* profile);

    void deleteExistingFiles();
    void installConfigs();
    void extractConfigs();
    void downloadMods();
    bool extractMods(const QMap<QString, VersionMod>& toExtract,
                     const QMap<QString, VersionMod>& toDecomp,
                     const QMap<QString, QString>& toCopy);
    void install();

   private:
    UserInteractionSupport* m_support;

    bool abortable = false;

    NetJob::Ptr jobPtr;

    InstallMode m_install_mode;
    QString m_pack_name;
    QString m_pack_safe_name;
    QString m_version_name;
    PackVersion m_version;

    QMap<QString, VersionMod> modsToExtract;
    QMap<QString, VersionMod> modsToDecomp;
    QMap<QString, QString> modsToCopy;

    QString archivePath;
    QStringList jarmods;
    Meta::Version::Ptr minecraftVersion;
    QMap<QString, Meta::Version::Ptr> componentsToInstall;

    QFuture<std::optional<QStringList>> m_extractFuture;
    QFutureWatcher<std::optional<QStringList>> m_extractFutureWatcher;

    QFuture<bool> m_modExtractFuture;
    QFutureWatcher<bool> m_modExtractFutureWatcher;
};

}  // namespace ATLauncher
