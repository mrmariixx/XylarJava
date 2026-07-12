#pragma once

#include "FtbFilterModel.h"
#include "FtbListModel.h"

#include <QWidget>

#include "Application.h"
#include "tasks/Task.h"
#include "ui/pages/BasePage.h"
#include "ui/pages/modplatform/ModpackProviderBasePage.h"

namespace Ui {
class FtbPage;
}

class NewInstanceDialog;

class FtbPage : public QWidget, public ModpackProviderBasePage {
    Q_OBJECT

   public:
    explicit FtbPage(NewInstanceDialog* dialog, QWidget* parent = 0);
    virtual ~FtbPage();
    virtual QString displayName() const override { return "FTB"; }
    virtual QIcon icon() const override { return QIcon::fromTheme("ftb_logo"); }
    virtual QString id() const override { return "ftb"; }
    virtual QString helpPage() const override { return "FTB-platform"; }
    virtual bool shouldDisplay() const override;
    void retranslate() override;

    void openedImpl() override;
    void closedImpl() override;

    bool eventFilter(QObject* watched, QEvent* event) override;

    /** Programatically set the term in the search bar. */
    virtual void setSearchTerm(QString) override;
    /** Get the current term in the search bar. */
    [[nodiscard]] virtual QString getSerachTerm() const override;

   private:
    void suggestCurrent();

   private slots:
    void triggerSearch();

    void onSortingSelectionChanged(QString selected);
    void onSelectionChanged(QModelIndex first, QModelIndex second);
    void onVersionSelectionChanged(QString selected);

   private:
    Ui::FtbPage* m_ui = nullptr;
    NewInstanceDialog* m_dialog = nullptr;
    Ftb::ListModel* m_listModel = nullptr;
    Ftb::FilterModel* m_filterModel = nullptr;

    FTB::Modpack m_selected;
    QString m_selectedVersion;

    bool m_initialised{ false };
};
