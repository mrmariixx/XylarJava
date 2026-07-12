#pragma once

#include <QWidget>
#include <memory>
#include "ui/pages/BasePage.h"

class LanguageSelectionWidget;

class LanguagePage : public QWidget, public BasePage {
    Q_OBJECT

   public:
    explicit LanguagePage(QWidget* parent = 0);
    virtual ~LanguagePage();

    QString displayName() const override { return tr("Language"); }
    QIcon icon() const override { return QIcon::fromTheme("language"); }
    QString id() const override { return "language-settings"; }
    QString helpPage() const override { return "Language-settings"; }
    bool apply() override;

    void retranslate() override;

   private:
    void applySettings();
    void loadSettings();

   private:
    LanguageSelectionWidget* mainWidget;
};
