#pragma once

#include <QPointer>

#include "ExternalResourcesPage.h"
#include "ui/dialogs/ResourceDownloadDialog.h"
#include "ui_ExternalResourcesPage.h"

#include "minecraft/mod/ResourcePackFolderModel.h"

class ResourcePackPage : public ExternalResourcesPage {
    Q_OBJECT
   public:
    explicit ResourcePackPage(MinecraftInstance* instance, ResourcePackFolderModel* model, QWidget* parent = 0);

    QString displayName() const override { return tr("Resource Packs"); }
    QIcon icon() const override { return QIcon::fromTheme("resourcepacks"); }
    QString id() const override { return "resourcepacks"; }
    QString helpPage() const override { return "Resource-packs"; }

    virtual bool shouldDisplay() const override
    {
        return !m_instance->traits().contains("no-texturepacks") && !m_instance->traits().contains("texturepacks");
    }

   public slots:
    void updateFrame(const QModelIndex& current, const QModelIndex& previous) override;

   private slots:
    void downloadResourcePacks();
    void downloadDialogFinished(int result);
    void updateResourcePacks();
    void deleteResourcePackMetadata();
    void changeResourcePackVersion();

   protected:
    ResourcePackFolderModel* m_model;
    QPointer<ResourceDownload::ResourceDownloadDialog> m_downloadDialog;
};
