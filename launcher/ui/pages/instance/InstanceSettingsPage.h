#pragma once

#include <QWidget>
#include "BaseInstance.h"
#include "ui/pages/BasePage.h"
#include "ui/widgets/MinecraftSettingsWidget.h"

class InstanceSettingsPage : public MinecraftSettingsWidget, public BasePage {
    Q_OBJECT

   public:
    explicit InstanceSettingsPage(MinecraftInstance* instance, QWidget* parent = nullptr) : MinecraftSettingsWidget(instance, parent)
    {
        connect(APPLICATION, &Application::globalSettingsAboutToOpen, this, &InstanceSettingsPage::saveSettings);
        connect(APPLICATION, &Application::globalSettingsApplied, this, &InstanceSettingsPage::loadSettings);
    }
    ~InstanceSettingsPage() override {}
    QString displayName() const override { return tr("Settings"); }
    QIcon icon() const override { return QIcon::fromTheme("instance-settings"); }
    QString id() const override { return "settings"; }
    bool apply() override
    {
        saveSettings();
        return true;
    }
    QString helpPage() const override { return "Instance-settings"; }
};
