#include "ModrinthResourcePages.h"
#include "ui/pages/modplatform/DataPackModel.h"
#include "ui_ResourcePage.h"

#include "modplatform/modrinth/ModrinthAPI.h"

#include "ui/dialogs/ResourceDownloadDialog.h"

namespace ResourceDownload {

ModrinthModPage::ModrinthModPage(ModDownloadDialog* dialog, BaseInstance& instance) : ModPage(dialog, instance)
{
    m_model = new ModModel(instance, new ModrinthAPI(), Modrinth::debugName(), Modrinth::metaEntryBase());
    m_ui->packView->setModel(m_model);

    addSortings();

    // sometimes Qt just ignores virtual slots and doesn't work as intended it seems,
    // so it's best not to connect them in the parent's constructor...
    connect(m_ui->sortByBox, &QComboBox::currentIndexChanged, this, &ModrinthModPage::triggerSearch);
    connect(m_ui->packView->selectionModel(), &QItemSelectionModel::currentChanged, this, &ModrinthModPage::onSelectionChanged);
    connect(m_ui->versionSelectionBox, &QComboBox::currentIndexChanged, this, &ModrinthModPage::onVersionSelectionChanged);
    connect(m_ui->resourceSelectionButton, &QPushButton::clicked, this, &ModrinthModPage::onResourceSelected);

    m_ui->packDescription->setMetaEntry(metaEntryBase());
}

ModrinthResourcePackPage::ModrinthResourcePackPage(ResourcePackDownloadDialog* dialog, BaseInstance& instance)
    : ResourcePackResourcePage(dialog, instance)
{
    m_model = new ResourcePackResourceModel(instance, new ModrinthAPI(), Modrinth::debugName(), Modrinth::metaEntryBase());
    m_ui->packView->setModel(m_model);

    addSortings();

    // sometimes Qt just ignores virtual slots and doesn't work as intended it seems,
    // so it's best not to connect them in the parent's constructor...
    connect(m_ui->sortByBox, &QComboBox::currentIndexChanged, this, &ModrinthResourcePackPage::triggerSearch);
    connect(m_ui->packView->selectionModel(), &QItemSelectionModel::currentChanged, this, &ModrinthResourcePackPage::onSelectionChanged);
    connect(m_ui->versionSelectionBox, &QComboBox::currentIndexChanged, this, &ModrinthResourcePackPage::onVersionSelectionChanged);
    connect(m_ui->resourceSelectionButton, &QPushButton::clicked, this, &ModrinthResourcePackPage::onResourceSelected);

    m_ui->packDescription->setMetaEntry(metaEntryBase());
}

ModrinthTexturePackPage::ModrinthTexturePackPage(TexturePackDownloadDialog* dialog, BaseInstance& instance)
    : TexturePackResourcePage(dialog, instance)
{
    m_model = new TexturePackResourceModel(instance, new ModrinthAPI(), Modrinth::debugName(), Modrinth::metaEntryBase());
    m_ui->packView->setModel(m_model);

    addSortings();

    // sometimes Qt just ignores virtual slots and doesn't work as intended it seems,
    // so it's best not to connect them in the parent's constructor...
    connect(m_ui->sortByBox, &QComboBox::currentIndexChanged, this, &ModrinthTexturePackPage::triggerSearch);
    connect(m_ui->packView->selectionModel(), &QItemSelectionModel::currentChanged, this, &ModrinthTexturePackPage::onSelectionChanged);
    connect(m_ui->versionSelectionBox, &QComboBox::currentIndexChanged, this, &ModrinthTexturePackPage::onVersionSelectionChanged);
    connect(m_ui->resourceSelectionButton, &QPushButton::clicked, this, &ModrinthTexturePackPage::onResourceSelected);

    m_ui->packDescription->setMetaEntry(metaEntryBase());
}

ModrinthShaderPackPage::ModrinthShaderPackPage(ShaderPackDownloadDialog* dialog, BaseInstance& instance)
    : ShaderPackResourcePage(dialog, instance)
{
    m_model = new ShaderPackResourceModel(instance, new ModrinthAPI(), Modrinth::debugName(), Modrinth::metaEntryBase());
    m_ui->packView->setModel(m_model);

    addSortings();

    // sometimes Qt just ignores virtual slots and doesn't work as intended it seems,
    // so it's best not to connect them in the parent's constructor...
    connect(m_ui->sortByBox, &QComboBox::currentIndexChanged, this, &ModrinthShaderPackPage::triggerSearch);
    connect(m_ui->packView->selectionModel(), &QItemSelectionModel::currentChanged, this, &ModrinthShaderPackPage::onSelectionChanged);
    connect(m_ui->versionSelectionBox, &QComboBox::currentIndexChanged, this, &ModrinthShaderPackPage::onVersionSelectionChanged);
    connect(m_ui->resourceSelectionButton, &QPushButton::clicked, this, &ModrinthShaderPackPage::onResourceSelected);

    m_ui->packDescription->setMetaEntry(metaEntryBase());
}

ModrinthDataPackPage::ModrinthDataPackPage(DataPackDownloadDialog* dialog, BaseInstance& instance) : DataPackResourcePage(dialog, instance)
{
    m_model = new DataPackResourceModel(instance, new ModrinthAPI(), Modrinth::debugName(), Modrinth::metaEntryBase());
    m_ui->packView->setModel(m_model);

    addSortings();

    // sometimes Qt just ignores virtual slots and doesn't work as intended it seems,
    // so it's best not to connect them in the parent's constructor...
    connect(m_ui->sortByBox, &QComboBox::currentIndexChanged, this, &ModrinthDataPackPage::triggerSearch);
    connect(m_ui->packView->selectionModel(), &QItemSelectionModel::currentChanged, this, &ModrinthDataPackPage::onSelectionChanged);
    connect(m_ui->versionSelectionBox, &QComboBox::currentIndexChanged, this, &ModrinthDataPackPage::onVersionSelectionChanged);
    connect(m_ui->resourceSelectionButton, &QPushButton::clicked, this, &ModrinthDataPackPage::onResourceSelected);

    m_ui->packDescription->setMetaEntry(metaEntryBase());
}

// I don't know why, but doing this on the parent class makes it so that
// other mod providers start loading before being selected, at least with
// my Qt, so we need to implement this in every derived class...
auto ModrinthModPage::shouldDisplay() const -> bool
{
    return true;
}
auto ModrinthResourcePackPage::shouldDisplay() const -> bool
{
    return true;
}
auto ModrinthTexturePackPage::shouldDisplay() const -> bool
{
    return true;
}
auto ModrinthShaderPackPage::shouldDisplay() const -> bool
{
    return true;
}
auto ModrinthDataPackPage::shouldDisplay() const -> bool
{
    return true;
}

std::unique_ptr<ModFilterWidget> ModrinthModPage::createFilterWidget()
{
    return ModFilterWidget::create(&static_cast<MinecraftInstance&>(m_baseInstance), true);
}

void ModrinthModPage::prepareProviderCategories()
{
    auto [categoriesTask, response] = ModrinthAPI::getModCategories();
    m_categoriesTask = categoriesTask;
    connect(m_categoriesTask.get(), &Task::succeeded, [this, response]() {
        auto categories = ModrinthAPI::loadModCategories(*response);
        m_filter_widget->setCategories(categories);
    });
    m_categoriesTask->start();
};
}  // namespace ResourceDownload
