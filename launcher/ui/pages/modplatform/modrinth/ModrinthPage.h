#pragma once

#include "modplatform/ModIndex.h"
#include "modplatform/modrinth/ModrinthAPI.h"
#include "ui/dialogs/NewInstanceDialog.h"

#include "ui/pages/modplatform/ModpackProviderBasePage.h"
#include "ui/widgets/ModFilterWidget.h"
#include "ui/widgets/ProgressWidget.h"

#include <QTimer>
#include <QWidget>

namespace Ui {
class ModrinthPage;
}

namespace Modrinth {
class ModpackListModel;
}

class ModrinthPage : public QWidget, public ModpackProviderBasePage {
    Q_OBJECT

   public:
    explicit ModrinthPage(NewInstanceDialog* dialog, QWidget* parent = nullptr);
    ~ModrinthPage() override;

    QString displayName() const override { return tr("Modrinth"); }
    QIcon icon() const override { return QIcon::fromTheme("modrinth"); }
    QString id() const override { return "modrinth"; }
    QString helpPage() const override { return "Modrinth-platform"; }

    inline QString debugName() const { return "Modrinth"; }
    inline QString metaEntryBase() const { return "ModrinthModpacks"; };

    ModPlatform::IndexedPack::Ptr getCurrent() { return m_current; }
    void suggestCurrent();

    void updateUI();

    void retranslate() override;
    void openedImpl() override;
    bool eventFilter(QObject* watched, QEvent* event) override;

    /** Programatically set the term in the search bar. */
    virtual void setSearchTerm(QString) override;
    /** Get the current term in the search bar. */
    virtual QString getSerachTerm() const override;

   private slots:
    void onSelectionChanged(QModelIndex first, QModelIndex second);
    void onVersionSelectionChanged(int index);
    void triggerSearch();
    void createFilterWidget();

   private:
    Ui::ModrinthPage* m_ui;
    NewInstanceDialog* m_dialog;
    Modrinth::ModpackListModel* m_model;

    ModPlatform::IndexedPack::Ptr m_current;
    QString m_selectedVersion;

    ProgressWidget m_fetch_progress;

    // Used to do instant searching with a delay to cache quick changes
    QTimer m_search_timer;

    std::unique_ptr<ModFilterWidget> m_filterWidget;
    Task::Ptr m_categoriesTask;

    ModrinthAPI m_api;
    Task::Ptr m_job;
    Task::Ptr m_job2;
};
