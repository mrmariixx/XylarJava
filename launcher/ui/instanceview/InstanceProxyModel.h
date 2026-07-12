#pragma once

#include <QCollator>
#include <QSortFilterProxyModel>

class InstanceProxyModel : public QSortFilterProxyModel {
    Q_OBJECT

   public:
    InstanceProxyModel(QObject* parent = 0);

   protected:
    QVariant data(const QModelIndex& index, int role) const override;
    bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;
    bool subSortLessThan(const QModelIndex& left, const QModelIndex& right) const;

   private:
    QCollator m_naturalSort;
};
