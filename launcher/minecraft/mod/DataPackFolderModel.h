#pragma once

#include "ResourceFolderModel.h"

#include "DataPack.h"
#include "ResourcePack.h"

class DataPackFolderModel : public ResourceFolderModel {
    Q_OBJECT
   public:
    enum Columns { ActiveColumn = 0, ImageColumn, NameColumn, PackFormatColumn, DateColumn, NUM_COLUMNS };

    explicit DataPackFolderModel(const QString& dir, BaseInstance* instance, bool is_indexed, bool create_dir, QObject* parent = nullptr);

    virtual QString id() const override { return "datapacks"; }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int columnCount(const QModelIndex& parent) const override;

    [[nodiscard]] Resource* createResource(const QFileInfo& file) override;
    [[nodiscard]] Task* createParseTask(Resource&) override;

    RESOURCE_HELPERS(DataPack)
};
