#pragma once

#include <QDialog>
#include <memory>

#include "java/JavaChecker.h"
#include "ui/pages/BasePage.h"
#include "ui/widgets/MinecraftSettingsWidget.h"

class SettingsObject;

class MinecraftPage : public MinecraftSettingsWidget, public BasePage {
    Q_OBJECT

   public:
    explicit MinecraftPage(QWidget* parent = nullptr) : MinecraftSettingsWidget(nullptr, parent) {}
    ~MinecraftPage() override {}

    QString displayName() const override { return tr("Minecraft"); }
    QIcon icon() const override { return QIcon::fromTheme("minecraft"); }
    QString id() const override { return "minecraft-settings"; }
    QString helpPage() const override { return "Minecraft-settings"; }
    bool apply() override
    {
        saveSettings();
        return true;
    }
};
