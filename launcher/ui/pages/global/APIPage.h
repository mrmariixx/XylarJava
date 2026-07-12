#pragma once

#include <QWidget>

#include "ui/pages/BasePage.h"

namespace Ui {
class APIPage;
}

class APIPage : public QWidget, public BasePage {
    Q_OBJECT

   public:
    explicit APIPage(QWidget* parent = 0);
    ~APIPage();

    QString displayName() const override { return tr("Services"); }
    QIcon icon() const override { return QIcon::fromTheme("worlds"); }
    QString id() const override { return "apis"; }
    QString helpPage() const override { return "APIs"; }
    virtual bool apply() override;
    void retranslate() override;

   private:
    int baseURLPasteType;
    void resetBaseURLNote();
    void updateBaseURLNote(int index);
    void updateBaseURLPlaceholder(int index);
    void loadSettings();
    void applySettings();

   private:
    Ui::APIPage* ui;
};
