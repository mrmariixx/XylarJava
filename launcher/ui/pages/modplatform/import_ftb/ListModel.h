#pragma once

#include <QAbstractListModel>
#include <QIcon>
#include <QSortFilterProxyModel>
#include <QVariant>
#include "modplatform/import_ftb/PackHelpers.h"

namespace FTBImportAPP {

class FilterModel : public QSortFilterProxyModel {
    Q_OBJECT
   public:
    FilterModel(QObject* parent = Q_NULLPTR);
    enum Sorting { ByName, ByGameVersion };
    const QMap<QString, Sorting> getAvailableSortings();
    QString translateCurrentSorting();
    void setSorting(Sorting sorting);
    Sorting getCurrentSorting();
    void setSearchTerm(QString term);

   protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
    bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;

   private:
    QMap<QString, Sorting> m_sortings;
    Sorting m_currentSorting;
    QString m_searchTerm;
};

class ListModel : public QAbstractListModel {
    Q_OBJECT

   public:
    ListModel(QObject* parent);
    virtual ~ListModel() = default;

    int rowCount(const QModelIndex& parent) const { return m_modpacks.size(); }
    int columnCount(const QModelIndex& parent) const { return 1; }
    QVariant data(const QModelIndex& index, int role) const;

    void update();

    QString getUserPath();
    void setPath(QString path);

   private:
    ModpackList m_modpacks;
    const QString m_instances_path;
};
}  // namespace FTBImportAPP
