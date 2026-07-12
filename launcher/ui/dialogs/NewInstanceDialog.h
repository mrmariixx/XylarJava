#pragma once

#include <QDialog>

#include "InstanceTask.h"
#include "ui/pages/BasePageProvider.h"

namespace Ui {
class NewInstanceDialog;
}

class PageContainer;
class QDialogButtonBox;
class ImportPage;
class FlamePage;

class NewInstanceDialog : public QDialog, public BasePageProvider {
    Q_OBJECT

   public:
    explicit NewInstanceDialog(const QString& initialGroup,
                               const QString& url = QString(),
                               const QMap<QString, QString>& extra_info = {},
                               QWidget* parent = 0);
    ~NewInstanceDialog();

    void updateDialogState();

    void setSuggestedPack(const QString& name = QString(), InstanceTask* task = nullptr);
    void setSuggestedPack(const QString& name, QString version, InstanceTask* task = nullptr);
    void setSuggestedIconFromFile(const QString& path, const QString& name);
    void setSuggestedIcon(const QString& key);

    InstanceTask* extractTask();

    QString dialogTitle() override;
    QList<BasePage*> getPages() override;

    QString instName() const;
    QString instGroup() const;
    QString iconKey() const;

   public slots:
    void accept() override;
    void reject() override;

   private slots:
    void on_iconButton_clicked();
    void on_instNameTextBox_textChanged(const QString& arg1);
    void selectedPageChanged(BasePage* previous, BasePage* selected);

   private:
    Ui::NewInstanceDialog* ui = nullptr;
    PageContainer* m_container = nullptr;
    QDialogButtonBox* m_buttons = nullptr;

    QString InstIconKey;
    ImportPage* importPage = nullptr;
    std::unique_ptr<InstanceTask> creationTask;

    bool importIcon = false;
    QString importIconPath;
    QString importIconName;

    QString importVersion;

    QString m_searchTerm;

    void importIconNow();
};
