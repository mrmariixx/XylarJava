#pragma once

#include "ResourceFolderModel.h"

#include "TexturePack.h"

class TexturePackFolderModel : public ResourceFolderModel {
    Q_OBJECT

   public:
    enum Columns { ActiveColumn = 0, ImageColumn, NameColumn, DateColumn, ProviderColumn, SizeColumn, NUM_COLUMNS };

    explicit TexturePackFolderModel(const QDir& dir, BaseInstance* instance, bool is_indexed, bool create_dir, QObject* parent = nullptr);

    virtual QString id() const override { return "texturepacks"; }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int columnCount(const QModelIndex& parent) const override;

    [[nodiscard]] Resource* createResource(const QFileInfo& file) override { return new TexturePack(file); }
    [[nodiscard]] Task* createParseTask(Resource&) override;

    RESOURCE_HELPERS(TexturePack)
};
