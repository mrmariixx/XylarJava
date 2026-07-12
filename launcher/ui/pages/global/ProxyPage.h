#pragma once

#include <QAbstractButton>
#include <QDialog>
#include <memory>

#include "ui/pages/BasePage.h"

namespace Ui {
class ProxyPage;
}

class ProxyPage : public QWidget, public BasePage {
    Q_OBJECT

   public:
    explicit ProxyPage(QWidget* parent = 0);
    ~ProxyPage();

    QString displayName() const override { return tr("Proxy"); }
    QIcon icon() const override { return QIcon::fromTheme("proxy"); }
    QString id() const override { return "proxy-settings"; }
    QString helpPage() const override { return "Proxy-settings"; }
    bool apply() override;
    void retranslate() override;

   private slots:
    void proxyGroupChanged(QAbstractButton* button);

   private:
    void updateCheckboxStuff();
    void applySettings();
    void loadSettings();

   private:
    Ui::ProxyPage* ui;
};
