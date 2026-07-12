#pragma once

#include <QLineEdit>
#include <QSortFilterProxyModel>
#include <QWidget>
#include "BaseVersionList.h"
#include "Filter.h"
#include "VersionListView.h"

class VersionProxyModel;
class VersionListView;
class QVBoxLayout;
class QProgressBar;

class VersionSelectWidget : public QWidget {
    Q_OBJECT
   public:
    explicit VersionSelectWidget(QWidget* parent);
    ~VersionSelectWidget();

    //! loads the list if needed.
    void initialize(BaseVersionList* vlist, bool forceLoad = false);

    //! Starts a task that loads the list.
    void loadList();

    bool hasVersions() const;
    BaseVersion::Ptr selectedVersion() const;
    void selectRecommended();
    void selectCurrent();
    void selectSearch();
    VersionListView* view();

    void setCurrentVersion(const QString& version);
    void setFuzzyFilter(BaseVersionList::ModelRoles role, QString filter);
    void setExactFilter(BaseVersionList::ModelRoles role, QString filter);
    void setExactIfPresentFilter(BaseVersionList::ModelRoles role, QString filter);
    void setFilter(BaseVersionList::ModelRoles role, Filter filter);
    void setEmptyString(QString emptyString);
    void setEmptyErrorString(QString emptyErrorString);
    void setEmptyMode(VersionListView::EmptyMode mode);
    void setResizeOn(int column);

    bool eventFilter(QObject* watched, QEvent* event) override;

   signals:
    void selectedVersionChanged(BaseVersion::Ptr version);

   protected:
    virtual void closeEvent(QCloseEvent*) override;

   private slots:
    void onTaskSucceeded();
    void onTaskFailed(const QString& reason);
    void changeProgress(qint64 current, qint64 total);
    void currentRowChanged(const QModelIndex& current, const QModelIndex&);

   private:
    void preselect();

   private:
    QString m_currentVersion;
    BaseVersionList* m_vlist = nullptr;
    VersionProxyModel* m_proxyModel = nullptr;
    int resizeOnColumn = 0;
    Task::Ptr m_load_task;
    bool preselectedAlready = false;

    QVBoxLayout* verticalLayout = nullptr;
    VersionListView* listView = nullptr;
    QLineEdit* search;
    QProgressBar* sneakyProgressBar = nullptr;
};
