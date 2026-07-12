#pragma once

#include <QPointer>
#include "ExternalResourcesPage.h"
#include "ui/dialogs/ResourceDownloadDialog.h"

class ShaderPackPage : public ExternalResourcesPage {
    Q_OBJECT
   public:
    explicit ShaderPackPage(MinecraftInstance* instance, ShaderPackFolderModel* model, QWidget* parent = nullptr);
    ~ShaderPackPage() override = default;

    QString displayName() const override { return tr("Shader Packs"); }
    QIcon icon() const override { return QIcon::fromTheme("shaderpacks"); }
    QString id() const override { return "shaderpacks"; }
    QString helpPage() const override { return "shader-packs"; }

    bool shouldDisplay() const override { return true; }

   public slots:
    void downloadShaderPack();
    void downloadDialogFinished(int result);
    void updateShaderPacks();
    void deleteShaderPackMetadata();
    void changeShaderPackVersion();

   private:
    ShaderPackFolderModel* m_model;
    QPointer<ResourceDownload::ShaderPackDownloadDialog> m_downloadDialog;
};
