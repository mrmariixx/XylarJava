#pragma once

#include <QCache>
#include <QStyledItemDelegate>

class ListViewDelegate : public QStyledItemDelegate {
    Q_OBJECT

   public:
    explicit ListViewDelegate(QObject* parent = 0);
    virtual ~ListViewDelegate() {}

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

   signals:
    void textChanged(QString before, QString after) const;

   private slots:
    void editingDone();
};
