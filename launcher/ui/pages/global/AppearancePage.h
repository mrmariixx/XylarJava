#pragma once

#include <QDialog>
#include <QLayout>
#include "java/JavaChecker.h"
#include "translations/TranslationsModel.h"
#include "ui/pages/BasePage.h"
#include "ui/widgets/AppearanceWidget.h"

class QTextCharFormat;
class SettingsObject;

class AppearancePage : public AppearanceWidget, public BasePage {
    Q_OBJECT

   public:
    explicit AppearancePage(QWidget* parent = nullptr) : AppearanceWidget(false, parent) { layout()->setContentsMargins(0, 0, 6, 0); }

    QString displayName() const override { return tr("Appearance"); }
    QIcon icon() const override { return QIcon::fromTheme("appearance"); }
    QString id() const override { return "appearance-settings"; }
    QString helpPage() const override { return "Launcher-settings"; }

    bool apply() override
    {
        applySettings();
        return true;
    }

    void retranslate() override { retranslateUi(); }
};
