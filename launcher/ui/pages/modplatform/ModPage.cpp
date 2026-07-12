#include "ModPage.h"
#include "ui_ResourcePage.h"

#include <QDesktopServices>
#include <QKeyEvent>
#include <QRegularExpression>

#include <memory>

#include "Application.h"
#include "ResourceDownloadTask.h"

#include "minecraft/MinecraftInstance.h"
#include "minecraft/PackProfile.h"

#include "ui/dialogs/ResourceDownloadDialog.h"

namespace ResourceDownload {

ModPage::ModPage(ModDownloadDialog* dialog, BaseInstance& instance) : ResourcePage(dialog, instance)
{
    connect(m_ui->resourceFilterButton, &QPushButton::clicked, this, &ModPage::filterMods);
}

void ModPage::setFilterWidget(std::unique_ptr<ModFilterWidget>& widget)
{
    if (m_filter_widget)
        disconnect(m_filter_widget.get(), nullptr, nullptr, nullptr);

    auto old = m_ui->splitter->replaceWidget(0, widget.get());
    // because we replaced the widget we also need to delete it
    if (old) {
        delete old;
    }

    m_filter_widget.swap(widget);

    m_filter = m_filter_widget->getFilter();

    connect(m_filter_widget.get(), &ModFilterWidget::filterChanged, this, &ModPage::triggerSearch);
    prepareProviderCategories();
}

/******** Callbacks to events in the UI (set up in the derived classes) ********/

void ModPage::filterMods()
{
    m_filter_widget->setHidden(!m_filter_widget->isHidden());
}

void ModPage::triggerSearch()
{
    auto changed = m_filter_widget->changed();
    m_filter = m_filter_widget->getFilter();
    m_ui->packView->selectionModel()->setCurrentIndex({}, QItemSelectionModel::SelectionFlag::ClearAndSelect);
    m_ui->packView->clearSelection();
    m_ui->packDescription->clear();
    m_ui->versionSelectionBox->clear();
    updateSelectionButton();

    static_cast<ModModel*>(m_model)->searchWithTerm(getSearchTerm(), m_ui->sortByBox->currentData().toUInt(), changed);
    m_fetchProgress.watch(m_model->activeSearchJob().get());
}

QMap<QString, QString> ModPage::urlHandlers() const
{
    QMap<QString, QString> map;
    map.insert(QRegularExpression::anchoredPattern("(?:www\\.)?modrinth\\.com\\/mod\\/([^\\/]+)\\/?"), "modrinth");
    map.insert(QRegularExpression::anchoredPattern("(?:www\\.)?curseforge\\.com\\/minecraft\\/mc-mods\\/([^\\/]+)\\/?"), "curseforge");
    map.insert(QRegularExpression::anchoredPattern("minecraft\\.curseforge\\.com\\/projects\\/([^\\/]+)\\/?"), "curseforge");
    return map;
}

/******** Make changes to the UI ********/

void ModPage::addResourceToPage(ModPlatform::IndexedPack::Ptr pack, ModPlatform::IndexedVersion& version, ResourceFolderModel* base_model)
{
    bool is_indexed = !APPLICATION->settings()->get("ModMetadataDisabled").toBool();
    m_model->addPack(pack, version, base_model, is_indexed);
}

}  // namespace ResourceDownload
