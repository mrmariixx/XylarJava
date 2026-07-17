#pragma once

#include <QDateTime>
#include <QHash>
#include <QList>
#include <QSet>
#include <QString>
#include <QStringList>

#include <meta/JsonFormat.h>
#include <memory>
#include "Agent.h"
#include "Library.h"
#include "ProblemProvider.h"
#include "java/JavaMetadata.h"
#include "minecraft/Rule.h"

class PackProfile;
class VersionFile;
class LaunchProfile;
struct MojangDownloadInfo;
struct MojangAssetIndexInfo;

using VersionFilePtr = std::shared_ptr<VersionFile>;
class VersionFile : public ProblemContainer {
    friend class MojangVersionFormat;
    friend class OneSixVersionFormat;

   public: /* methods */
    void applyTo(LaunchProfile* profile, const RuntimeContext& runtimeContext);

   public: /* data */
<<<<<<< HEAD
    /// PolyMC: order hint for this version file if no explicit order is set
    int order = 0;

    /// PolyMC: human readable name of this package
    QString name;

    /// PolyMC: package ID of this package
    QString uid;

    /// PolyMC: version of this package
    QString version;

    /// PolyMC: DEPRECATED dependency on a Minecraft version
=======
    /// Prism Launcher: order hint for this version file if no explicit order is set
    int order = 0;

    /// Prism Launcher: human readable name of this package
    QString name;

    /// Prism Launcher: package ID of this package
    QString uid;

    /// Prism Launcher: version of this package
    QString version;

    /// Prism Launcher: DEPRECATED dependency on a Minecraft version
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
    QString dependsOnMinecraftVersion;

    /// Mojang: DEPRECATED used to version the Mojang version format
    int minimumLauncherVersion = -1;

    /// Mojang: DEPRECATED version of Minecraft this is
    QString minecraftVersion;

    /// Mojang: class to launch Minecraft with
    QString mainClass;

<<<<<<< HEAD
    /// PolyMC: class to launch legacy Minecraft with (embed in a custom window)
=======
    /// Prism Launcher: class to launch legacy Minecraft with (embed in a custom window)
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
    QString appletClass;

    /// Mojang: Minecraft launch arguments (may contain placeholders for variable substitution)
    QString minecraftArguments;

<<<<<<< HEAD
    /// PolyMC: Additional JVM launch arguments
=======
    /// Prism Launcher: Additional JVM launch arguments
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
    QStringList addnJvmArguments;

    /// Mojang: list of compatible java majors
    QList<int> compatibleJavaMajors;

    /// Mojang: the name of recommended java version
    QString compatibleJavaName;

    /// Mojang: type of the Minecraft version
    QString type;

    /// Mojang: the time this version was actually released by Mojang
    QDateTime releaseTime;

    /// Mojang: DEPRECATED the time this version was last updated by Mojang
    QDateTime updateTime;

    /// Mojang: DEPRECATED asset group to be used with Minecraft
    QString assets;

<<<<<<< HEAD
    /// PolyMC: list of tweaker mod arguments for launchwrapper
=======
    /// Prism Launcher: list of tweaker mod arguments for launchwrapper
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
    QStringList addTweakers;

    /// Mojang: list of libraries to add to the version
    QList<LibraryPtr> libraries;

<<<<<<< HEAD
    /// PolyMC: list of maven files to put in the libraries folder, but not in classpath
    QList<LibraryPtr> mavenFiles;

    /// PolyMC: list of agents to add to JVM arguments
=======
    /// Prism Launcher: list of maven files to put in the libraries folder, but not in classpath
    QList<LibraryPtr> mavenFiles;

    /// Prism Launcher: list of agents to add to JVM arguments
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
    QList<Agent> agents;

    /// The main jar (Minecraft version library, normally)
    LibraryPtr mainJar;

<<<<<<< HEAD
    /// PolyMC: list of attached traits of this version file - used to enable features
    QSet<QString> traits;

    /// PolyMC: list of jar mods added to this version
    QList<LibraryPtr> jarMods;

    /// PolyMC: list of mods added to this version
    QList<LibraryPtr> mods;

    /**
     * PolyMC: set of packages this depends on
=======
    /// Prism Launcher: list of attached traits of this version file - used to enable features
    QSet<QString> traits;

    /// Prism Launcher: list of jar mods added to this version
    QList<LibraryPtr> jarMods;

    /// Prism Launcher: list of mods added to this version
    QList<LibraryPtr> mods;

    /**
     * Prism Launcher: set of packages this depends on
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
     * NOTE: this is shared with the meta format!!!
     */
    Meta::RequireSet m_requires;

    /**
<<<<<<< HEAD
     * PolyMC: set of packages this conflicts with
=======
     * Prism Launcher: set of packages this conflicts with
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
     * NOTE: this is shared with the meta format!!!
     */
    Meta::RequireSet conflicts;

    /// is volatile -- may be removed as soon as it is no longer needed by something else
    bool m_volatile = false;

    QList<Java::MetadataPtr> runtimes;

   public:
    // Mojang: DEPRECATED list of 'downloads' - client jar, server jar, windows server exe, maybe more.
    QMap<QString, std::shared_ptr<MojangDownloadInfo>> mojangDownloads;

    // Mojang: extended asset index download information
    std::shared_ptr<MojangAssetIndexInfo> mojangAssetIndex;
};
