#pragma once

#include <QWidget>

#include "ui/pages/BasePage.h"

namespace Ui {
class ExternalToolsPage;
}

class ExternalToolsPage : public QWidget, public BasePage {
    Q_OBJECT

   public:
    explicit ExternalToolsPage(QWidget* parent = 0);
    ~ExternalToolsPage();

    QString displayName() const override { return tr("Tools"); }
    QIcon icon() const override
    {
        auto icon = QIcon::fromTheme("externaltools");
        if (icon.isNull()) {
            icon = QIcon::fromTheme("loadermods");
        }
        return icon;
    }
    QString id() const override { return "external-tools"; }
    QString helpPage() const override { return "Tools"; }
    virtual bool apply() override;
    void retranslate() override;

   private:
    void loadSettings();
    void applySettings();

   private:
    Ui::ExternalToolsPage* ui;

   private slots:
    void on_jprofilerPathBtn_clicked();
    void on_jprofilerCheckBtn_clicked();
    void on_jvisualvmPathBtn_clicked();
    void on_jvisualvmCheckBtn_clicked();
    void on_mceditPathBtn_clicked();
    void on_mceditCheckBtn_clicked();
    void on_jsonEditorBrowseBtn_clicked();
};
