#pragma once

#include <QTimer>
#include <QWidget>

#include "TechnicData.h"
#include "net/NetJob.h"
#include "ui/pages/modplatform/ModpackProviderBasePage.h"
#include "ui/widgets/ProgressWidget.h"

namespace Ui {
class TechnicPage;
}

class NewInstanceDialog;

namespace Technic {
class ListModel;
}

class TechnicPage : public QWidget, public ModpackProviderBasePage {
    Q_OBJECT

   public:
    explicit TechnicPage(NewInstanceDialog* dialog, QWidget* parent = 0);
    virtual ~TechnicPage();
    virtual QString displayName() const override { return "Technic"; }
    virtual QIcon icon() const override { return QIcon::fromTheme("technic"); }
    virtual QString id() const override { return "technic"; }
    virtual QString helpPage() const override { return "Technic-platform"; }
    virtual bool shouldDisplay() const override;
    void retranslate() override;

    void openedImpl() override;

    bool eventFilter(QObject* watched, QEvent* event) override;

    /** Programatically set the term in the search bar. */
    virtual void setSearchTerm(QString) override;
    /** Get the current term in the search bar. */
    virtual QString getSerachTerm() const override;

   private:
    void suggestCurrent();
    void metadataLoaded();
    void selectVersion();

   private slots:
    void triggerSearch();
    void onSelectionChanged(QModelIndex first, QModelIndex second);
    void onSolderLoaded(QByteArray* responsePtr);
    void onVersionSelectionChanged(QString data);

   private:
    Ui::TechnicPage* ui = nullptr;
    NewInstanceDialog* dialog = nullptr;
    Technic::ListModel* model = nullptr;

    Technic::Modpack current;
    QString selectedVersion;

    NetJob::Ptr jobPtr;

    ProgressWidget m_fetch_progress;

    // Used to do instant searching with a delay to cache quick changes
    QTimer m_search_timer;
};
