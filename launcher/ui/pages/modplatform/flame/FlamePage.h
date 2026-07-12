#pragma once

#include <QWidget>

#include <QTimer>
#include "modplatform/ModIndex.h"
#include "ui/pages/modplatform/ModpackProviderBasePage.h"
#include "ui/widgets/ModFilterWidget.h"
#include "ui/widgets/ProgressWidget.h"

namespace Ui {
class FlamePage;
}

class NewInstanceDialog;

namespace Flame {
class ListModel;
}

class FlamePage : public QWidget, public ModpackProviderBasePage {
    Q_OBJECT

   public:
    explicit FlamePage(NewInstanceDialog* dialog, QWidget* parent = 0);
    virtual ~FlamePage();
    virtual QString displayName() const override { return "CurseForge"; }
    virtual QIcon icon() const override { return QIcon::fromTheme("flame"); }
    virtual QString id() const override { return "flame"; }
    virtual QString helpPage() const override { return "Flame-platform"; }
    virtual bool shouldDisplay() const override;
    void retranslate() override;

    void updateUi();

    void openedImpl() override;

    bool eventFilter(QObject* watched, QEvent* event) override;

    /** Programatically set the term in the search bar. */
    virtual void setSearchTerm(QString) override;
    /** Get the current term in the search bar. */
    virtual QString getSerachTerm() const override;

   private:
    void suggestCurrent();

   private slots:
    void triggerSearch();
    void onSelectionChanged(QModelIndex first, QModelIndex second);
    void onVersionSelectionChanged(int index);
    void createFilterWidget();

   private:
    Ui::FlamePage* m_ui = nullptr;
    NewInstanceDialog* m_dialog = nullptr;
    Flame::ListModel* m_listModel = nullptr;
    ModPlatform::IndexedPack::Ptr m_current;

    int m_selected_version_index = -1;

    ProgressWidget m_fetch_progress;

    // Used to do instant searching with a delay to cache quick changes
    QTimer m_search_timer;

    std::unique_ptr<ModFilterWidget> m_filterWidget;
    Task::Ptr m_categoriesTask;
    Task::Ptr m_job;
};
