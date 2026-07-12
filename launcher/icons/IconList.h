#pragma once

#include <QAbstractListModel>
#include <QDir>
#include <QFile>
#include <QMutex>
#include <QtGui/QIcon>
#include <memory>

#include "MMCIcon.h"
#include "settings/Setting.h"

#include "QObjectPtr.h"

class QFileSystemWatcher;

class IconList : public QAbstractListModel {
    Q_OBJECT
   public:
    explicit IconList(const QStringList& builtinPaths, const QString& path, QObject* parent = 0);
    virtual ~IconList() {};

    QIcon getIcon(const QString& key) const;
    int getIconIndex(const QString& key) const;
    QString getDirectory() const;

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    virtual QStringList mimeTypes() const override;
    virtual Qt::DropActions supportedDropActions() const override;
    virtual bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;

    bool addThemeIcon(const QString& key);
    bool addIcon(const QString& key, const QString& name, const QString& path, IconType type);
    void saveIcon(const QString& key, const QString& path, const char* format) const;
    bool deleteIcon(const QString& key);
    bool trashIcon(const QString& key);
    bool iconFileExists(const QString& key) const;
    QString iconDirectory(const QString& key) const;

    void installIcons(const QStringList& iconFiles);
    void installIcon(const QString& file, const QString& name);

    const MMCIcon* icon(const QString& key) const;

    void startWatching();
    void stopWatching();

   signals:
    void iconUpdated(QString key);

   private:
    // hide copy constructor
    IconList(const IconList&) = delete;
    // hide assign op
    IconList& operator=(const IconList&) = delete;
    void reindex();
    void sortIconList();
    bool addPathRecursively(const QString& path);
    QStringList getIconFilePaths() const;

   public slots:
    void directoryChanged(const QString& path);

   protected slots:
    void fileChanged(const QString& path);
    void SettingChanged(const Setting& setting, const QVariant& value);

   private:
    shared_qobject_ptr<QFileSystemWatcher> m_watcher;
    bool m_isWatching;
    QMap<QString, int> m_nameIndex;
    QList<MMCIcon> m_icons;
    QDir m_dir;
};
