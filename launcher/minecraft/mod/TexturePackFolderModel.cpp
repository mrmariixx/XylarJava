#include "TexturePackFolderModel.h"

#include "minecraft/mod/tasks/LocalTexturePackParseTask.h"
#include "minecraft/mod/tasks/ResourceFolderLoadTask.h"

TexturePackFolderModel::TexturePackFolderModel(const QDir& dir, BaseInstance* instance, bool is_indexed, bool create_dir, QObject* parent)
    : ResourceFolderModel(QDir(dir), instance, is_indexed, create_dir, parent)
{
    m_column_names = QStringList({ "Enable", "Image", "Name", "Last Modified", "Provider", "Size" });
    m_column_names_translated = QStringList({ tr("Enable"), tr("Image"), tr("Name"), tr("Last Modified"), tr("Provider"), tr("Size") });
    m_column_sort_keys = { SortType::ENABLED, SortType::NAME, SortType::NAME, SortType::DATE, SortType::PROVIDER, SortType::SIZE };
    m_column_resize_modes = { QHeaderView::Interactive, QHeaderView::Interactive, QHeaderView::Stretch,
                              QHeaderView::Interactive, QHeaderView::Interactive, QHeaderView::Interactive };
    m_columnsHideable = { false, true, false, true, true, true };
}

Task* TexturePackFolderModel::createParseTask(Resource& resource)
{
    return new LocalTexturePackParseTask(m_next_resolution_ticket, static_cast<TexturePack&>(resource));
}

QVariant TexturePackFolderModel::data(const QModelIndex& index, int role) const
{
    if (!validateIndex(index))
        return {};

    int row = index.row();
    int column = index.column();

    switch (role) {
        case Qt::BackgroundRole:
            return rowBackground(row);
        case Qt::DecorationRole: {
            if (column == ImageColumn) {
                return at(row).image({ 32, 32 }, Qt::AspectRatioMode::KeepAspectRatioByExpanding);
            }
            break;
        }
        case Qt::SizeHintRole:
            if (column == ImageColumn) {
                return QSize(32, 32);
            }
            break;
    }

    // map the columns to the base equivilents
    QModelIndex mappedIndex;
    switch (column) {
        case ActiveColumn:
            mappedIndex = index.siblingAtColumn(ResourceFolderModel::ActiveColumn);
            break;
        case NameColumn:
            mappedIndex = index.siblingAtColumn(ResourceFolderModel::NameColumn);
            break;
        case DateColumn:
            mappedIndex = index.siblingAtColumn(ResourceFolderModel::DateColumn);
            break;
        case ProviderColumn:
            mappedIndex = index.siblingAtColumn(ResourceFolderModel::ProviderColumn);
            break;
        case SizeColumn:
            mappedIndex = index.siblingAtColumn(ResourceFolderModel::SizeColumn);
            break;
    }

    if (mappedIndex.isValid()) {
        return ResourceFolderModel::data(mappedIndex, role);
    }

    return {};
}

QVariant TexturePackFolderModel::headerData(int section, [[maybe_unused]] Qt::Orientation orientation, int role) const
{
    switch (role) {
        case Qt::DisplayRole:
            switch (section) {
                case ActiveColumn:
                case NameColumn:
                case DateColumn:
                case ImageColumn:
                case ProviderColumn:
                case SizeColumn:
                    return columnNames().at(section);
                default:
                    return {};
            }
        case Qt::ToolTipRole: {
            switch (section) {
                case ActiveColumn:
                    return tr("Is the texture pack enabled?");
                case NameColumn:
                    return tr("The name of the texture pack.");
                case DateColumn:
                    return tr("The date and time this texture pack was last changed (or added).");
                case ProviderColumn:
                    return tr("The source provider of the texture pack.");
                case SizeColumn:
                    return tr("The size of the texture pack.");
                default:
                    return {};
            }
        }
        default:
            break;
    }

    return {};
}

int TexturePackFolderModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : NUM_COLUMNS;
}
