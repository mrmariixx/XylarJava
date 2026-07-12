#include "FlameResourcePages.h"
#include <QList>
#include <memory>
#include "modplatform/flame/FlameAPI.h"
#include "ui_ResourcePage.h"

#include "FlameResourceModels.h"
#include "ui/dialogs/ResourceDownloadDialog.h"

namespace ResourceDownload {

FlameModPage::FlameModPage(ModDownloadDialog* dialog, BaseInstance& instance) : ModPage(dialog, instance)
{
    m_model = new ModModel(instance, new FlameAPI(), Flame::debugName(), Flame::metaEntryBase());
    m_ui->packView->setModel(m_model);

    addSortings();

    // sometimes Qt just ignores virtual slots and doesn't work as intended it seems,
    // so it's best not to connect them in the parent's contructor...
    connect(m_ui->sortByBox, &QComboBox::currentIndexChanged, this, &FlameModPage::triggerSearch);
    connect(m_ui->packView->selectionModel(), &QItemSelectionModel::currentChanged, this, &FlameModPage::onSelectionChanged);
    connect(m_ui->versionSelectionBox, &QComboBox::currentIndexChanged, this, &FlameModPage::onVersionSelectionChanged);
    connect(m_ui->resourceSelectionButton, &QPushButton::clicked, this, &FlameModPage::onResourceSelected);

    m_ui->packDescription->setMetaEntry(metaEntryBase());
}

void FlameModPage::openUrl(const QUrl& url)
{
    if (url.scheme().isEmpty()) {
        QString query = url.query(QUrl::FullyDecoded);

        if (query.startsWith("remoteUrl=")) {
            // attempt to resolve url from warning page
            query.remove(0, 10);
            ModPage::openUrl({ QUrl::fromPercentEncoding(query.toUtf8()) });  // double decoding is necessary
            return;
        }
    }

    ModPage::openUrl(url);
}

FlameResourcePackPage::FlameResourcePackPage(ResourcePackDownloadDialog* dialog, BaseInstance& instance)
    : ResourcePackResourcePage(dialog, instance)
{
    m_model = new ResourcePackResourceModel(instance, new FlameAPI(), Flame::debugName(), Flame::metaEntryBase());
    m_ui->packView->setModel(m_model);

    addSortings();

    // sometimes Qt just ignores virtual slots and doesn't work as intended it seems,
    // so it's best not to connect them in the parent's contructor...
    connect(m_ui->sortByBox, &QComboBox::currentIndexChanged, this, &FlameResourcePackPage::triggerSearch);
    connect(m_ui->packView->selectionModel(), &QItemSelectionModel::currentChanged, this, &FlameResourcePackPage::onSelectionChanged);
    connect(m_ui->versionSelectionBox, &QComboBox::currentIndexChanged, this, &FlameResourcePackPage::onVersionSelectionChanged);
    connect(m_ui->resourceSelectionButton, &QPushButton::clicked, this, &FlameResourcePackPage::onResourceSelected);

    m_ui->packDescription->setMetaEntry(metaEntryBase());
}

void FlameResourcePackPage::openUrl(const QUrl& url)
{
    if (url.scheme().isEmpty()) {
        QString query = url.query(QUrl::FullyDecoded);

        if (query.startsWith("remoteUrl=")) {
            // attempt to resolve url from warning page
            query.remove(0, 10);
            ResourcePackResourcePage::openUrl({ QUrl::fromPercentEncoding(query.toUtf8()) });  // double decoding is necessary
            return;
        }
    }

    ResourcePackResourcePage::openUrl(url);
}

FlameTexturePackPage::FlameTexturePackPage(TexturePackDownloadDialog* dialog, BaseInstance& instance)
    : TexturePackResourcePage(dialog, instance)
{
    m_model = new FlameTexturePackModel(instance);
    m_ui->packView->setModel(m_model);

    addSortings();

    // sometimes Qt just ignores virtual slots and doesn't work as intended it seems,
    // so it's best not to connect them in the parent's contructor...
    connect(m_ui->sortByBox, &QComboBox::currentIndexChanged, this, &FlameTexturePackPage::triggerSearch);
    connect(m_ui->packView->selectionModel(), &QItemSelectionModel::currentChanged, this, &FlameTexturePackPage::onSelectionChanged);
    connect(m_ui->versionSelectionBox, &QComboBox::currentIndexChanged, this, &FlameTexturePackPage::onVersionSelectionChanged);
    connect(m_ui->resourceSelectionButton, &QPushButton::clicked, this, &FlameTexturePackPage::onResourceSelected);

    m_ui->packDescription->setMetaEntry(metaEntryBase());
}

void FlameTexturePackPage::openUrl(const QUrl& url)
{
    if (url.scheme().isEmpty()) {
        QString query = url.query(QUrl::FullyDecoded);

        if (query.startsWith("remoteUrl=")) {
            // attempt to resolve url from warning page
            query.remove(0, 10);
            ResourcePackResourcePage::openUrl({ QUrl::fromPercentEncoding(query.toUtf8()) });  // double decoding is necessary
            return;
        }
    }

    TexturePackResourcePage::openUrl(url);
}

void FlameDataPackPage::openUrl(const QUrl& url)
{
    if (url.scheme().isEmpty()) {
        QString query = url.query(QUrl::FullyDecoded);

        if (query.startsWith("remoteUrl=")) {
            // attempt to resolve url from warning page
            query.remove(0, 10);
            DataPackResourcePage::openUrl({ QUrl::fromPercentEncoding(query.toUtf8()) });  // double decoding is necessary
            return;
        }
    }

    DataPackResourcePage::openUrl(url);
}

FlameShaderPackPage::FlameShaderPackPage(ShaderPackDownloadDialog* dialog, BaseInstance& instance)
    : ShaderPackResourcePage(dialog, instance)
{
    m_model = new ShaderPackResourceModel(instance, new FlameAPI(), Flame::debugName(), Flame::metaEntryBase());
    m_ui->packView->setModel(m_model);

    addSortings();

    // sometimes Qt just ignores virtual slots and doesn't work as intended it seems,
    // so it's best not to connect them in the parent's constructor...
    connect(m_ui->sortByBox, &QComboBox::currentIndexChanged, this, &FlameShaderPackPage::triggerSearch);
    connect(m_ui->packView->selectionModel(), &QItemSelectionModel::currentChanged, this, &FlameShaderPackPage::onSelectionChanged);
    connect(m_ui->versionSelectionBox, &QComboBox::currentIndexChanged, this, &FlameShaderPackPage::onVersionSelectionChanged);
    connect(m_ui->resourceSelectionButton, &QPushButton::clicked, this, &FlameShaderPackPage::onResourceSelected);

    m_ui->packDescription->setMetaEntry(metaEntryBase());
}

FlameDataPackPage::FlameDataPackPage(DataPackDownloadDialog* dialog, BaseInstance& instance) : DataPackResourcePage(dialog, instance)
{
    m_model = new DataPackResourceModel(instance, new FlameAPI(), Flame::debugName(), Flame::metaEntryBase());
    m_ui->packView->setModel(m_model);

    addSortings();

    // sometimes Qt just ignores virtual slots and doesn't work as intended it seems,
    // so it's best not to connect them in the parent's constructor...
    connect(m_ui->sortByBox, &QComboBox::currentIndexChanged, this, &FlameDataPackPage::triggerSearch);
    connect(m_ui->packView->selectionModel(), &QItemSelectionModel::currentChanged, this, &FlameDataPackPage::onSelectionChanged);
    connect(m_ui->versionSelectionBox, &QComboBox::currentIndexChanged, this, &FlameDataPackPage::onVersionSelectionChanged);
    connect(m_ui->resourceSelectionButton, &QPushButton::clicked, this, &FlameDataPackPage::onResourceSelected);

    m_ui->packDescription->setMetaEntry(metaEntryBase());
}

void FlameShaderPackPage::openUrl(const QUrl& url)
{
    if (url.scheme().isEmpty()) {
        QString query = url.query(QUrl::FullyDecoded);

        if (query.startsWith("remoteUrl=")) {
            // attempt to resolve url from warning page
            query.remove(0, 10);
            ShaderPackResourcePage::openUrl({ QUrl::fromPercentEncoding(query.toUtf8()) });  // double decoding is necessary
            return;
        }
    }

    ShaderPackResourcePage::openUrl(url);
}

// I don't know why, but doing this on the parent class makes it so that
// other mod providers start loading before being selected, at least with
// my Qt, so we need to implement this in every derived class...
auto FlameModPage::shouldDisplay() const -> bool
{
    return true;
}
auto FlameResourcePackPage::shouldDisplay() const -> bool
{
    return true;
}
auto FlameTexturePackPage::shouldDisplay() const -> bool
{
    return true;
}
auto FlameShaderPackPage::shouldDisplay() const -> bool
{
    return true;
}
auto FlameDataPackPage::shouldDisplay() const -> bool
{
    return true;
}

std::unique_ptr<ModFilterWidget> FlameModPage::createFilterWidget()
{
    return ModFilterWidget::create(&static_cast<MinecraftInstance&>(m_baseInstance), false);
}

void FlameModPage::prepareProviderCategories()
{
    auto [task, response] = FlameAPI::getModCategories();
    m_categoriesTask = task;
    connect(m_categoriesTask.get(), &Task::succeeded, [this, response]() {
        auto categories = FlameAPI::loadModCategories(*response);
        m_filter_widget->setCategories(categories);
    });
    m_categoriesTask->start();
};
}  // namespace ResourceDownload
