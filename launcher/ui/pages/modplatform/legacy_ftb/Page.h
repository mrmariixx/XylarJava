#pragma once

#include <QTextBrowser>
#include <QTreeView>
#include <QWidget>

#include "QObjectPtr.h"
#include "modplatform/legacy_ftb/PackFetchTask.h"
#include "modplatform/legacy_ftb/PackHelpers.h"
#include "ui/pages/modplatform/ModpackProviderBasePage.h"

class NewInstanceDialog;

namespace LegacyFTB {

namespace Ui {
class Page;
}

class ListModel;
class FilterModel;
class PrivatePackManager;

class Page : public QWidget, public ModpackProviderBasePage {
    Q_OBJECT

   public:
    explicit Page(NewInstanceDialog* dialog, QWidget* parent = 0);
    virtual ~Page();
    QString displayName() const override { return "FTB Legacy"; }
    QIcon icon() const override { return QIcon::fromTheme("ftb_logo"); }
    QString id() const override { return "legacy_ftb"; }
    QString helpPage() const override { return "FTB-legacy"; }
    bool shouldDisplay() const override;
    void openedImpl() override;
    void retranslate() override;

    /** Programatically set the term in the search bar. */
    virtual void setSearchTerm(QString) override;
    /** Get the current term in the search bar. */
    virtual QString getSerachTerm() const override;

   private:
    void suggestCurrent();
    void onPackSelectionChanged(Modpack* pack = nullptr);

   private slots:
    void ftbPackDataDownloadSuccessfully(ModpackList publicPacks, ModpackList thirdPartyPacks);
    void ftbPackDataDownloadFailed(QString reason);
    void ftbPackDataDownloadAborted();

    void ftbPrivatePackDataDownloadSuccessfully(const Modpack& pack);
    void ftbPrivatePackDataDownloadFailed(QString reason, QString packCode);

    void onSortingSelectionChanged(QString data);
    void onVersionSelectionItemChanged(QString data);

    void onPublicPackSelectionChanged(QModelIndex first, QModelIndex second);
    void onThirdPartyPackSelectionChanged(QModelIndex first, QModelIndex second);
    void onPrivatePackSelectionChanged(QModelIndex first, QModelIndex second);

    void onTabChanged(int tab);

    void onAddPackClicked();
    void onRemovePackClicked();

    void triggerSearch();

   private:
    FilterModel* currentModel = nullptr;
    QTreeView* currentList = nullptr;
    QTextBrowser* currentModpackInfo = nullptr;

    bool initialized = false;
    Modpack selected;
    QString selectedVersion;

    ListModel* publicListModel = nullptr;
    FilterModel* publicFilterModel = nullptr;

    ListModel* thirdPartyModel = nullptr;
    FilterModel* thirdPartyFilterModel = nullptr;

    ListModel* privateListModel = nullptr;
    FilterModel* privateFilterModel = nullptr;

    unique_qobject_ptr<PackFetchTask> ftbFetchTask;
    std::unique_ptr<PrivatePackManager> ftbPrivatePacks;

    NewInstanceDialog* dialog = nullptr;

    Ui::Page* ui = nullptr;
};

}  // namespace LegacyFTB
