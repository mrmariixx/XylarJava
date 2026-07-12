#pragma once
#include <QHeaderView>
#include <QTreeView>

class ModListView : public QTreeView {
    Q_OBJECT
   public:
    explicit ModListView(QWidget* parent = 0);
    virtual void setModel(QAbstractItemModel* model);
    virtual void setResizeModes(const QList<QHeaderView::ResizeMode>& modes);
};
