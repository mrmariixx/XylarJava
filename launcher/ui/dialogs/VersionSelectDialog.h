#pragma once

#include <QDialog>
#include <QSortFilterProxyModel>

#include "BaseVersionList.h"

class QVBoxLayout;
class QHBoxLayout;
class QDialogButtonBox;
class VersionSelectWidget;
class QPushButton;

class VersionProxyModel;

class VersionSelectDialog : public QDialog {
    Q_OBJECT

   public:
    explicit VersionSelectDialog(BaseVersionList* vlist, QString title, QWidget* parent = 0, bool cancelable = true);
    virtual ~VersionSelectDialog() = default;

    int exec() override;

    BaseVersion::Ptr selectedVersion() const;

    void setCurrentVersion(const QString& version);
    void setFuzzyFilter(BaseVersionList::ModelRoles role, QString filter);
    void setExactFilter(BaseVersionList::ModelRoles role, QString filter);
    void setExactIfPresentFilter(BaseVersionList::ModelRoles role, QString filter);
    void setEmptyString(QString emptyString);
    void setEmptyErrorString(QString emptyErrorString);
    void setResizeOn(int column);

   private slots:
    void on_refreshButton_clicked();

   private:
    void retranslate();
    void selectRecommended();

   private:
    QString m_currentVersion;
    VersionSelectWidget* m_versionWidget = nullptr;
    QVBoxLayout* m_verticalLayout = nullptr;
    QHBoxLayout* m_horizontalLayout = nullptr;
    QPushButton* m_refreshButton = nullptr;
    QDialogButtonBox* m_buttonBox = nullptr;

    BaseVersionList* m_vlist = nullptr;

    VersionProxyModel* m_proxyModel = nullptr;

    int resizeOnColumn = -1;

    Task* loadTask = nullptr;
};
