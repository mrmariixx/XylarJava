#pragma once

#include <QWidget>
#include "BaseInstance.h"
#include "JavaCommon.h"

namespace Ui {
class JavaSettingsWidget;
}

class JavaSettingsWidget : public QWidget {
    Q_OBJECT

   public:
    explicit JavaSettingsWidget(QWidget* parent = nullptr) : JavaSettingsWidget(nullptr, parent) {}
    explicit JavaSettingsWidget(BaseInstance* instance, QWidget* parent = nullptr);
    ~JavaSettingsWidget() override;

    void loadSettings();
    void saveSettings();

   private slots:
    void onJavaBrowse();
    void onJavaAutodetect();
    void onJavaTest();
    void updateThresholds();

   private:
    BaseInstance* m_instance;
    Ui::JavaSettingsWidget* m_ui;
    unique_qobject_ptr<JavaCommon::TestCheck> m_checker;
};
