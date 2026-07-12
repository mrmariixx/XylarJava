#pragma once

#include <QPointer>
#include "ExternalResourcesPage.h"
#include "ui/dialogs/ResourceDownloadDialog.h"

class ModFolderPage : public ExternalResourcesPage {
    Q_OBJECT

    inline bool handleNoModLoader();

   public:
    explicit ModFolderPage(BaseInstance* inst, ModFolderModel* model, QWidget* parent = nullptr);
    virtual ~ModFolderPage() = default;

    void setFilter(const QString& filter) { m_fileSelectionFilter = filter; }

    virtual QString displayName() const override { return tr("Mods"); }
    virtual QIcon icon() const override { return QIcon::fromTheme("loadermods"); }
    virtual QString id() const override { return "mods"; }
    virtual QString helpPage() const override { return "Loader-mods"; }

    virtual bool shouldDisplay() const override;

   public slots:
    void updateFrame(const QModelIndex& current, const QModelIndex& previous) override;

   private slots:
    void removeItems(const QItemSelection& selection) override;

    void downloadMods();
    void downloadDialogFinished(int result);
    void updateMods(bool includeDeps = false);
    void deleteModMetadata();
    void exportModMetadata();
    void changeModVersion();

   protected:
    ModFolderModel* m_model;
    QPointer<ResourceDownload::ModDownloadDialog> m_downloadDialog;
};

class CoreModFolderPage : public ModFolderPage {
    Q_OBJECT
   public:
    explicit CoreModFolderPage(BaseInstance* inst, ModFolderModel* mods, QWidget* parent = 0);
    virtual ~CoreModFolderPage() = default;

    virtual QString displayName() const override { return tr("Core Mods"); }
    virtual QIcon icon() const override { return QIcon::fromTheme("coremods"); }
    virtual QString id() const override { return "coremods"; }
    virtual QString helpPage() const override { return "Core-mods"; }

    virtual bool shouldDisplay() const override;
};

class NilModFolderPage : public ModFolderPage {
    Q_OBJECT
   public:
    explicit NilModFolderPage(BaseInstance* inst, ModFolderModel* mods, QWidget* parent = 0);
    virtual ~NilModFolderPage() = default;

    virtual QString displayName() const override { return tr("Nilmods"); }
    virtual QIcon icon() const override { return QIcon::fromTheme("coremods"); }
    virtual QString id() const override { return "nilmods"; }
    virtual QString helpPage() const override { return "Nilmods"; }

    virtual bool shouldDisplay() const override;
};
