#pragma once

#include <QWidget>

#include "tasks/Task.h"
#include "ui/pages/BasePage.h"

namespace Ui {
class ImportPage;
}

class NewInstanceDialog;

class ImportPage : public QWidget, public BasePage {
    Q_OBJECT

   public:
    explicit ImportPage(NewInstanceDialog* dialog, QWidget* parent = 0);
    virtual ~ImportPage();
    virtual QString displayName() const override { return tr("Import"); }
    virtual QIcon icon() const override { return QIcon::fromTheme("viewfolder"); }
    virtual QString id() const override { return "import"; }
    virtual QString helpPage() const override { return "Zip-import"; }
    virtual bool shouldDisplay() const override;
    void retranslate() override;

    void setUrl(const QString& url);
    void openedImpl() override;
    void setExtraInfo(const QMap<QString, QString>& extra_info);
   private slots:
    void on_modpackBtn_clicked();
    void updateState();

   private:
    QUrl modpackUrl() const;

   private:
    Ui::ImportPage* ui = nullptr;
    NewInstanceDialog* dialog = nullptr;
    QMap<QString, QString> m_extra_info = {};
};
