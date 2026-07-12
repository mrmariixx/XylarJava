#pragma once

#include <QFileInfo>
#include <QSortFilterProxyModel>
#include "SeparatorPrefixTree.h"

class FileIgnoreProxy : public QSortFilterProxyModel {
    Q_OBJECT

   public:
    FileIgnoreProxy(QString root, QObject* parent);
    // NOTE: Sadly, we have to do sorting ourselves.
    bool lessThan(const QModelIndex& left, const QModelIndex& right) const;

    virtual Qt::ItemFlags flags(const QModelIndex& index) const;

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

    QString relPath(const QString& path) const;

    bool setFilterState(QModelIndex index, Qt::CheckState state);

    bool shouldExpand(QModelIndex index);

    void setBlockedPaths(QStringList paths);

    inline const SeparatorPrefixTree<'/'>& blockedPaths() const { return m_blocked; }
    inline SeparatorPrefixTree<'/'>& blockedPaths() { return m_blocked; }

    // list of file names that need to be removed completely from model
    inline QStringList& ignoreFilesWithName() { return m_ignoreFiles; }
    inline QStringList& ignoreFilesWithSuffix() { return m_ignoreFilesSuffixes; }
    // list of relative paths that need to be removed completely from model
    inline SeparatorPrefixTree<'/'>& ignoreFilesWithPath() { return m_ignoreFilePaths; }

    bool filterFile(const QFileInfo& fileName) const;

    void loadBlockedPathsFromFile(const QString& fileName);

    void saveBlockedPathsToFile(const QString& fileName);

   protected:
    bool filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const;
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;

    bool ignoreFile(QFileInfo file) const;

   private:
    const QString m_root;
    SeparatorPrefixTree<'/'> m_blocked;
    QStringList m_ignoreFiles;
    QStringList m_ignoreFilesSuffixes;
    SeparatorPrefixTree<'/'> m_ignoreFilePaths;
};
