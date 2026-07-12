#pragma once

#include <QDialog>
#include "BaseInstance.h"
#include "FastFileIconProvider.h"
#include "FileIgnoreProxy.h"
#include "minecraft/MinecraftInstance.h"
#include "modplatform/ModIndex.h"

namespace Ui {
class ExportPackDialog;
}

class ExportPackDialog : public QDialog {
    Q_OBJECT

   public:
    explicit ExportPackDialog(MinecraftInstance* instance,
                              QWidget* parent = nullptr,
                              ModPlatform::ResourceProvider provider = ModPlatform::ResourceProvider::MODRINTH);
    ~ExportPackDialog();

    void done(int result) override;
    void validate();

   private:
    QString ignoreFileName();

   private:
    MinecraftInstance* m_instance;
    Ui::ExportPackDialog* m_ui;
    FileIgnoreProxy* m_proxy;
    FastFileIconProvider m_icons;
    const ModPlatform::ResourceProvider m_provider;
};
