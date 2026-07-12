#pragma once

#include <QButtonGroup>
#include <QList>
#include <QListWidgetItem>
#include <QTabWidget>

#include "Version.h"

#include "VersionProxyModel.h"
#include "meta/VersionList.h"

#include "minecraft/MinecraftInstance.h"
#include "modplatform/ModIndex.h"

class MinecraftInstance;

namespace Ui {
class ModFilterWidget;
}

class ModFilterWidget : public QTabWidget {
    Q_OBJECT
   public:
    struct Filter {
        std::vector<Version> versions;
        std::vector<ModPlatform::IndexedVersionType> releases;
        ModPlatform::ModLoaderTypes loaders;
        ModPlatform::Side side;
        bool hideInstalled;
        QStringList categoryIds;
        bool openSource;

        bool operator==(const Filter& other) const
        {
            return hideInstalled == other.hideInstalled && side == other.side && loaders == other.loaders && versions == other.versions &&
                   releases == other.releases && categoryIds == other.categoryIds && openSource == other.openSource;
        }
        bool operator!=(const Filter& other) const { return !(*this == other); }

        bool checkMcVersions(QStringList value)
        {
            for (auto mcVersion : versions)
                if (value.contains(mcVersion.toString()))
                    return true;

            return versions.empty();
        }

        bool checkModpackFilters(const ModPlatform::IndexedVersion& v)
        {
            return ((!loaders || !v.loaders || loaders & v.loaders) &&  // loaders
                    (releases.empty() ||                                // releases
                     std::find(releases.cbegin(), releases.cend(), v.version_type) != releases.cend()) &&
                    checkMcVersions({ v.mcVersion }));  // gameVersion}
        }
    };

    static std::unique_ptr<ModFilterWidget> create(MinecraftInstance* instance, bool extended);
    virtual ~ModFilterWidget();

    auto getFilter() -> std::shared_ptr<Filter>;
    auto changed() const -> bool { return m_filter_changed; }

   signals:
    void filterChanged();

   public slots:
    void setCategories(const QList<ModPlatform::Category>&);

   private:
    ModFilterWidget(MinecraftInstance* instance, bool extendedSupport);

    void loadVersionList();
    void prepareBasicFilter();

   private slots:
    void onVersionFilterChanged(int);
    void onVersionFilterTextChanged(const QString& version);
    void onLoadersFilterChanged();
    void onSideFilterChanged();
    void onHideInstalledFilterChanged();
    void onShowAllVersionsChanged();
    void onOpenSourceFilterChanged();
    void onReleaseFilterChanged();
    void onShowMoreClicked();

   private:
    Ui::ModFilterWidget* ui;

    MinecraftInstance* m_instance = nullptr;
    std::shared_ptr<Filter> m_filter;
    bool m_filter_changed = false;

    Meta::VersionList::Ptr m_version_list;
    VersionProxyModel* m_versions_proxy = nullptr;

    QList<ModPlatform::Category> m_categories;
};
