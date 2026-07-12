#pragma once

#include <QPointer>

#include "ExternalResourcesPage.h"
#include "ui/dialogs/ResourceDownloadDialog.h"
#include "ui_ExternalResourcesPage.h"

#include "minecraft/mod/TexturePackFolderModel.h"

class TexturePackPage : public ExternalResourcesPage {
    Q_OBJECT
   public:
    explicit TexturePackPage(MinecraftInstance* instance, TexturePackFolderModel* model, QWidget* parent = nullptr);

    QString displayName() const override { return tr("Texture packs"); }
    QIcon icon() const override { return QIcon::fromTheme("resourcepacks"); }
    QString id() const override { return "texturepacks"; }
    QString helpPage() const override { return "Texture-packs"; }

    virtual bool shouldDisplay() const override { return m_instance->traits().contains("texturepacks"); }

   public slots:
    void updateFrame(const QModelIndex& current, const QModelIndex& previous) override;
    void downloadTexturePacks();
    void downloadDialogFinished(int result);
    void updateTexturePacks();
    void deleteTexturePackMetadata();
    void changeTexturePackVersion();

   private:
    TexturePackFolderModel* m_model;
    QPointer<ResourceDownload::TexturePackDownloadDialog> m_downloadDialog;
};
