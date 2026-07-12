#pragma once

#include <QDialog>
#include <QTextBrowser>
#include <QTreeView>
#include <QWidget>

#include "modplatform/import_ftb/PackHelpers.h"
#include "ui/pages/modplatform/ModpackProviderBasePage.h"
#include "ui/pages/modplatform/import_ftb/ListModel.h"

class NewInstanceDialog;

namespace FTBImportAPP {
namespace Ui {
class ImportFTBPage;
}

class ImportFTBPage : public QWidget, public ModpackProviderBasePage {
    Q_OBJECT

   public:
    explicit ImportFTBPage(NewInstanceDialog* dialog, QWidget* parent = 0);
    virtual ~ImportFTBPage();
    QString displayName() const override { return tr("FTB App Import"); }
    QIcon icon() const override { return QIcon::fromTheme("ftb_logo"); }
    QString id() const override { return "import_ftb"; }
    QString helpPage() const override { return "FTB-import"; }
    bool shouldDisplay() const override { return true; }
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
    void onSortingSelectionChanged(QString data);
    void onPublicPackSelectionChanged(QModelIndex first, QModelIndex second);
    void triggerSearch();

   private:
    bool initialized = false;
    Modpack selected;
    ListModel* listModel = nullptr;
    FilterModel* currentModel = nullptr;

    NewInstanceDialog* dialog = nullptr;
    Ui::ImportFTBPage* ui = nullptr;
};

}  // namespace FTBImportAPP
