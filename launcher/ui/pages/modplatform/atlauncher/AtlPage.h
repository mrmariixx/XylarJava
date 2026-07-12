#pragma once

#include "AtlFilterModel.h"
#include "AtlListModel.h"

#include <modplatform/atlauncher/ATLPackInstallTask.h>
#include <QWidget>

#include "ui/pages/modplatform/ModpackProviderBasePage.h"

namespace Ui {
class AtlPage;
}

class NewInstanceDialog;

class AtlPage : public QWidget, public ModpackProviderBasePage {
    Q_OBJECT

   public:
    explicit AtlPage(NewInstanceDialog* dialog, QWidget* parent = 0);
    virtual ~AtlPage();
    virtual QString displayName() const override { return "ATLauncher"; }
    virtual QIcon icon() const override { return QIcon::fromTheme("atlauncher"); }
    virtual QString id() const override { return "atl"; }
    virtual QString helpPage() const override { return "ATL-platform"; }
    virtual bool shouldDisplay() const override;
    void retranslate() override;

    void openedImpl() override;

    /** Programatically set the term in the search bar. */
    virtual void setSearchTerm(QString) override;
    /** Get the current term in the search bar. */
    virtual QString getSerachTerm() const override;

   private:
    void suggestCurrent();

   private slots:
    void triggerSearch();

    void onSortingSelectionChanged(QString data);

    void onSelectionChanged(QModelIndex first, QModelIndex second);
    void onVersionSelectionChanged(QString data);

   private:
    Ui::AtlPage* ui = nullptr;
    NewInstanceDialog* dialog = nullptr;
    Atl::ListModel* listModel = nullptr;
    Atl::FilterModel* filterModel = nullptr;

    ATLauncher::IndexedPack selected;
    QString selectedVersion;

    bool initialized = false;
};
