#pragma once

#include <QWidget>
#include "JavaSettingsWidget.h"
#include "minecraft/MinecraftInstance.h"

namespace Ui {
class MinecraftSettingsWidget;
}

class MinecraftSettingsWidget : public QWidget {
   public:
    MinecraftSettingsWidget(MinecraftInstance* instance, QWidget* parent = nullptr);
    ~MinecraftSettingsWidget() override;

    void loadSettings();
    void saveSettings();

   private:
    void openGlobalSettings();
    void updateAccountsMenu(SettingsObject& settings);
    bool isQuickPlaySupported();
   private slots:
    void saveSelectedLoaders();
    void saveDataPacksPath();
    void selectDataPacksFolder();

    MinecraftInstance* m_instance;
    Ui::MinecraftSettingsWidget* m_ui;
    JavaSettingsWidget* m_javaSettings = nullptr;
    bool m_quickPlaySingleplayer = false;
};
