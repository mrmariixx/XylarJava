#pragma once

#include "modplatform/ResourceAPI.h"

#include "ui/pages/modplatform/DataPackPage.h"
#include "ui/pages/modplatform/ModPage.h"
#include "ui/pages/modplatform/ResourcePackPage.h"
#include "ui/pages/modplatform/ShaderPackPage.h"
#include "ui/pages/modplatform/TexturePackPage.h"

namespace ResourceDownload {

namespace Modrinth {
static inline QString displayName()
{
    return "Modrinth";
}
static inline QIcon icon()
{
    return QIcon::fromTheme("modrinth");
}
static inline QString id()
{
    return "modrinth";
}
static inline QString debugName()
{
    return "Modrinth";
}
static inline QString metaEntryBase()
{
    return "ModrinthPacks";
}
}  // namespace Modrinth

class ModrinthModPage : public ModPage {
    Q_OBJECT

   public:
    static ModrinthModPage* create(ModDownloadDialog* dialog, BaseInstance& instance)
    {
        return ModPage::create<ModrinthModPage>(dialog, instance);
    }

    ModrinthModPage(ModDownloadDialog* dialog, BaseInstance& instance);
    ~ModrinthModPage() override = default;

    bool shouldDisplay() const override;

    inline auto displayName() const -> QString override { return Modrinth::displayName(); }
    inline auto icon() const -> QIcon override { return Modrinth::icon(); }
    inline auto id() const -> QString override { return Modrinth::id(); }
    inline auto debugName() const -> QString override { return Modrinth::debugName(); }
    inline auto metaEntryBase() const -> QString override { return Modrinth::metaEntryBase(); }

    inline auto helpPage() const -> QString override { return "Mod-platform"; }

    std::unique_ptr<ModFilterWidget> createFilterWidget() override;

   protected:
    virtual void prepareProviderCategories() override;
    Task::Ptr m_categoriesTask;
};

class ModrinthResourcePackPage : public ResourcePackResourcePage {
    Q_OBJECT

   public:
    static ModrinthResourcePackPage* create(ResourcePackDownloadDialog* dialog, BaseInstance& instance)
    {
        return ResourcePackResourcePage::create<ModrinthResourcePackPage>(dialog, instance);
    }

    ModrinthResourcePackPage(ResourcePackDownloadDialog* dialog, BaseInstance& instance);
    ~ModrinthResourcePackPage() override = default;

    bool shouldDisplay() const override;

    inline auto displayName() const -> QString override { return Modrinth::displayName(); }
    inline auto icon() const -> QIcon override { return Modrinth::icon(); }
    inline auto id() const -> QString override { return Modrinth::id(); }
    inline auto debugName() const -> QString override { return Modrinth::debugName(); }
    inline auto metaEntryBase() const -> QString override { return Modrinth::metaEntryBase(); }

    inline auto helpPage() const -> QString override { return ""; }
};

class ModrinthTexturePackPage : public TexturePackResourcePage {
    Q_OBJECT

   public:
    static ModrinthTexturePackPage* create(TexturePackDownloadDialog* dialog, BaseInstance& instance)
    {
        return TexturePackResourcePage::create<ModrinthTexturePackPage>(dialog, instance);
    }

    ModrinthTexturePackPage(TexturePackDownloadDialog* dialog, BaseInstance& instance);
    ~ModrinthTexturePackPage() override = default;

    bool shouldDisplay() const override;

    inline auto displayName() const -> QString override { return Modrinth::displayName(); }
    inline auto icon() const -> QIcon override { return Modrinth::icon(); }
    inline auto id() const -> QString override { return Modrinth::id(); }
    inline auto debugName() const -> QString override { return Modrinth::debugName(); }
    inline auto metaEntryBase() const -> QString override { return Modrinth::metaEntryBase(); }

    inline auto helpPage() const -> QString override { return ""; }
};

class ModrinthShaderPackPage : public ShaderPackResourcePage {
    Q_OBJECT

   public:
    static ModrinthShaderPackPage* create(ShaderPackDownloadDialog* dialog, BaseInstance& instance)
    {
        return ShaderPackResourcePage::create<ModrinthShaderPackPage>(dialog, instance);
    }

    ModrinthShaderPackPage(ShaderPackDownloadDialog* dialog, BaseInstance& instance);
    ~ModrinthShaderPackPage() override = default;

    bool shouldDisplay() const override;

    inline auto displayName() const -> QString override { return Modrinth::displayName(); }
    inline auto icon() const -> QIcon override { return Modrinth::icon(); }
    inline auto id() const -> QString override { return Modrinth::id(); }
    inline auto debugName() const -> QString override { return Modrinth::debugName(); }
    inline auto metaEntryBase() const -> QString override { return Modrinth::metaEntryBase(); }

    inline auto helpPage() const -> QString override { return ""; }
};

class ModrinthDataPackPage : public DataPackResourcePage {
    Q_OBJECT

   public:
    static ModrinthDataPackPage* create(DataPackDownloadDialog* dialog, BaseInstance& instance)
    {
        return DataPackResourcePage::create<ModrinthDataPackPage>(dialog, instance);
    }

    ModrinthDataPackPage(DataPackDownloadDialog* dialog, BaseInstance& instance);
    ~ModrinthDataPackPage() override = default;

    bool shouldDisplay() const override;

    inline auto displayName() const -> QString override { return Modrinth::displayName(); }
    inline auto icon() const -> QIcon override { return Modrinth::icon(); }
    inline auto id() const -> QString override { return Modrinth::id(); }
    inline auto debugName() const -> QString override { return Modrinth::debugName(); }
    inline auto metaEntryBase() const -> QString override { return Modrinth::metaEntryBase(); }

    inline auto helpPage() const -> QString override { return ""; }
};

}  // namespace ResourceDownload
