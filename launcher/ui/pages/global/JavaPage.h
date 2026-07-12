#pragma once

#include <QObjectPtr.h>
#include <QDialog>
#include <QStringListModel>
#include "JavaCommon.h"
#include "ui/pages/BasePage.h"
#include "ui/widgets/JavaSettingsWidget.h"

class SettingsObject;

namespace Ui {
class JavaPage;
}

class JavaPage : public QWidget, public BasePage {
    Q_OBJECT

   public:
    explicit JavaPage(QWidget* parent = 0);
    ~JavaPage();

    QString displayName() const override { return tr("Java"); }
    QIcon icon() const override { return QIcon::fromTheme("java"); }
    QString id() const override { return "java-settings"; }
    QString helpPage() const override { return "Java-settings"; }
    void retranslate() override;

    bool apply() override;

   private slots:
    void on_downloadJavaButton_clicked();
    void on_removeJavaButton_clicked();
    void on_refreshJavaButton_clicked();

   private:
    Ui::JavaPage* ui;
};
