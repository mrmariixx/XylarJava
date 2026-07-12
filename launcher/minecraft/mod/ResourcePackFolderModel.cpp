#include "ResourcePackFolderModel.h"

#include <QIcon>
#include <QStyle>

#include "Version.h"

#include "minecraft/mod/tasks/LocalDataPackParseTask.h"

ResourcePackFolderModel::ResourcePackFolderModel(const QDir& dir, BaseInstance* instance, bool is_indexed, bool create_dir, QObject* parent)
    : ResourceFolderModel(dir, instance, is_indexed, create_dir, parent)
{
    m_column_names = QStringList({ "Enable", "Image", "Name", "Pack Format", "Last Modified", "Provider", "Size" });
    m_column_names_translated =
        QStringList({ tr("Enable"), tr("Image"), tr("Name"), tr("Pack Format"), tr("Last Modified"), tr("Provider"), tr("Size") });
    m_column_sort_keys = { SortType::ENABLED, SortType::NAME,     SortType::NAME, SortType::PACK_FORMAT,
                           SortType::DATE,    SortType::PROVIDER, SortType::SIZE };
    m_column_resize_modes = { QHeaderView::Interactive, QHeaderView::Interactive, QHeaderView::Stretch,    QHeaderView::Interactive,
                              QHeaderView::Interactive, QHeaderView::Interactive, QHeaderView::Interactive };
    m_columnsHideable = { false, true, false, true, true, true, true };
}

QVariant ResourcePackFolderModel::data(const QModelIndex& index, int role) const
{
    if (!validateIndex(index))
        return {};

    int row = index.row();
    int column = index.column();

    switch (role) {
        case Qt::BackgroundRole:
            return rowBackground(row);
        case Qt::DisplayRole: {
            if (column == PackFormatColumn) {
                const auto& resource = at(row);
                return resource.packFormatStr();
            }
            break;
        }
        case Qt::DecorationRole: {
            if (column == ImageColumn) {
                return at(row).image({ 32, 32 }, Qt::AspectRatioMode::KeepAspectRatioByExpanding);
            }
            break;
        }
        case Qt::ToolTipRole: {
            if (column == PackFormatColumn) {
                //: The string being explained by this is in the format: ID (Lower version - Upper version)
                return tr("The resource pack format ID, as well as the Minecraft versions it was designed for.");
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

QVariant ResourcePackFolderModel::headerData(int section, [[maybe_unused]] Qt::Orientation orientation, int role) const
{
    switch (role) {
        case Qt::DisplayRole:
            switch (section) {
                case ActiveColumn:
                case NameColumn:
                case PackFormatColumn:
                case DateColumn:
                case ImageColumn:
                case ProviderColumn:
                case SizeColumn:
                    return columnNames().at(section);
                default:
                    return {};
            }

        case Qt::ToolTipRole:
            switch (section) {
                case ActiveColumn:
                    return tr("Is the resource pack enabled?");
                case NameColumn:
                    return tr("The name of the resource pack.");
                case PackFormatColumn:
                    //: The string being explained by this is in the format: ID (Lower version - Upper version)
                    return tr("The resource pack format ID, as well as the Minecraft versions it was designed for.");
                case DateColumn:
                    return tr("The date and time this resource pack was last changed (or added).");
                case ProviderColumn:
                    return tr("The source provider of the resource pack.");
                case SizeColumn:
                    return tr("The size of the resource pack.");
                default:
                    return {};
            }
        case Qt::SizeHintRole:
            if (section == ImageColumn) {
                return QSize(64, 0);
            }
            return {};
        default:
            return {};
    }
}

int ResourcePackFolderModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : NUM_COLUMNS;
}

Task* ResourcePackFolderModel::createParseTask(Resource& resource)
{
    return new LocalDataPackParseTask(m_next_resolution_ticket, dynamic_cast<ResourcePack*>(&resource));
}
