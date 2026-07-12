#include "DataPackFolderModel.h"

#include <QIcon>
#include <QStyle>

#include "Version.h"

#include "minecraft/mod/tasks/LocalDataPackParseTask.h"

DataPackFolderModel::DataPackFolderModel(const QString& dir, BaseInstance* instance, bool is_indexed, bool create_dir, QObject* parent)
    : ResourceFolderModel(QDir(dir), instance, is_indexed, create_dir, parent)
{
    m_column_names = QStringList({ "Enable", "Image", "Name", "Pack Format", "Last Modified" });
    m_column_names_translated = QStringList({ tr("Enable"), tr("Image"), tr("Name"), tr("Pack Format"), tr("Last Modified") });
    m_column_sort_keys = { SortType::ENABLED, SortType::NAME, SortType::NAME, SortType::PACK_FORMAT, SortType::DATE };
    m_column_resize_modes = { QHeaderView::Interactive, QHeaderView::Interactive, QHeaderView::Stretch, QHeaderView::Interactive,
                              QHeaderView::Interactive };
    m_columnsHideable = { false, true, false, true, true };
}

QVariant DataPackFolderModel::data(const QModelIndex& index, int role) const
{
    if (!validateIndex(index))
        return {};

    int row = index.row();
    int column = index.column();

    switch (role) {
        case Qt::BackgroundRole:
            return rowBackground(row);
        case Qt::DisplayRole:
            switch (column) {
                case PackFormatColumn: {
                    const auto& resource = at(row);
                    return resource.packFormatStr();
                }
            }
            break;
        case Qt::DecorationRole: {
            if (column == ImageColumn) {
                return at(row).image({ 32, 32 }, Qt::AspectRatioMode::KeepAspectRatioByExpanding);
            }
            break;
        }
        case Qt::ToolTipRole: {
            if (column == PackFormatColumn) {
                //: The string being explained by this is in the format: ID (Lower version - Upper version)
                return tr("The data pack format ID, as well as the Minecraft versions it was designed for.");
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
            // FIXME: there is no size column due to an oversight
    }

    if (mappedIndex.isValid()) {
        return ResourceFolderModel::data(mappedIndex, role);
    }

    return {};
}

QVariant DataPackFolderModel::headerData(int section, [[maybe_unused]] Qt::Orientation orientation, int role) const
{
    switch (role) {
        case Qt::DisplayRole:
            switch (section) {
                case ActiveColumn:
                case NameColumn:
                case PackFormatColumn:
                case DateColumn:
                case ImageColumn:
                    return columnNames().at(section);
                default:
                    return {};
            }

        case Qt::ToolTipRole:
            switch (section) {
                case ActiveColumn:
                    return tr("Is the data pack enabled? (Only valid for ZIPs)");
                case NameColumn:
                    return tr("The name of the data pack.");
                case PackFormatColumn:
                    //: The string being explained by this is in the format: ID (Lower version - Upper version)
                    return tr("The data pack format ID, as well as the Minecraft versions it was designed for.");
                case DateColumn:
                    return tr("The date and time this data pack was last changed (or added).");
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

int DataPackFolderModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : NUM_COLUMNS;
}

Resource* DataPackFolderModel::createResource(const QFileInfo& file)
{
    return new DataPack(file);
}

Task* DataPackFolderModel::createParseTask(Resource& resource)
{
    return new LocalDataPackParseTask(m_next_resolution_ticket, static_cast<DataPack*>(&resource));
}
