#pragma once

#include <QDebug>
#include <QList>
#include <QString>

#include <list>
#include <optional>
#include <utility>

#include "../Version.h"

#include "modplatform/ModIndex.h"
#include "modplatform/ResourceType.h"
#include "tasks/Task.h"

/* Simple class with a common interface for interacting with APIs */
class ResourceAPI {
   public:
    virtual ~ResourceAPI() = default;

    struct SortingMethod {
        // The index of the sorting method. Used to allow for arbitrary ordering in the list of methods.
        // Used by Flame in the API request.
        unsigned int index;
        // The real name of the sorting, as used in the respective API specification.
        // Used by Modrinth in the API request.
        QString name;
        // The human-readable name of the sorting, used for display in the UI.
        QString readable_name;
    };

    template <typename T>
    struct Callback {
        std::function<void(T&)> on_succeed;
        std::function<void(const QString& reason, int network_error_code)> on_fail;
        std::function<void()> on_abort;
    };

    struct SearchArgs {
        ModPlatform::ResourceType type{};
        int offset = 0;

        std::optional<QString> search;
        std::optional<SortingMethod> sorting;
        std::optional<ModPlatform::ModLoaderTypes> loaders;
        std::optional<std::vector<Version>> versions;
        std::optional<ModPlatform::Side> side;
        std::optional<QStringList> categoryIds;
        bool openSource{};
    };

    struct VersionSearchArgs {
        ModPlatform::IndexedPack::Ptr pack;

        std::optional<std::vector<Version>> mcVersions;
        std::optional<ModPlatform::ModLoaderTypes> loaders;
        ModPlatform::ResourceType resourceType;
        bool includeChangelog{};
    };

    struct ProjectInfoArgs {
        ModPlatform::IndexedPack::Ptr pack;
    };

    struct DependencySearchArgs {
        ModPlatform::Dependency dependency;
        Version mcVersion;
        ModPlatform::ModLoaderTypes loader;
        bool includeChangelog{};
    };

   public:
    /** Gets a list of available sorting methods for this API. */
    virtual auto getSortingMethods() const -> QList<SortingMethod> = 0;

   public slots:
    virtual Task::Ptr searchProjects(SearchArgs&&, Callback<QList<ModPlatform::IndexedPack::Ptr>>&&) const;

    virtual std::pair<Task::Ptr, QByteArray*> getProject(QString addonId) const;
    virtual std::pair<Task::Ptr, QByteArray*> getProjects(QStringList addonIds) const = 0;

    virtual Task::Ptr getProjectInfo(ProjectInfoArgs&&, Callback<ModPlatform::IndexedPack::Ptr>&&) const;
    Task::Ptr getProjectVersions(VersionSearchArgs&& args, Callback<QVector<ModPlatform::IndexedVersion>>&& callbacks) const;
    virtual Task::Ptr getDependencyVersion(DependencySearchArgs&&, Callback<ModPlatform::IndexedVersion>&&) const;

   protected:
    inline QString debugName() const { return "External resource API"; }

    QString mapMCVersionToModrinth(Version v) const;

    QString getGameVersionsString(std::vector<Version> mcVersions) const;

   public:
    virtual auto getSearchURL(const SearchArgs& args) const -> std::optional<QString> = 0;
    virtual auto getInfoURL(const QString& id) const -> std::optional<QString> = 0;
    virtual auto getVersionsURL(const VersionSearchArgs& args) const -> std::optional<QString> = 0;
    virtual auto getDependencyURL(const DependencySearchArgs& args) const -> std::optional<QString> = 0;

    /** Functions to load data into a pack.
     *
     *  Those are needed for the same reason as documentToArray, and NEED to be re-implemented in the same way.
     */

    virtual void loadIndexedPack(ModPlatform::IndexedPack&, QJsonObject&) const = 0;
    virtual ModPlatform::IndexedVersion loadIndexedPackVersion(QJsonObject& obj, ModPlatform::ResourceType) const = 0;

    /** Converts a JSON document to a common array format.
     *
     *  This is needed so that different providers, with different JSON structures, can be parsed
     *  uniformally. You NEED to re-implement this if you intend on using the default callbacks.
     */
    virtual QJsonArray documentToArray(QJsonDocument& obj) const = 0;

    /** Functions to load data into a pack.
     *
     *  Those are needed for the same reason as documentToArray, and NEED to be re-implemented in the same way.
     */

    virtual void loadExtraPackInfo(ModPlatform::IndexedPack&, QJsonObject&) const = 0;
};
