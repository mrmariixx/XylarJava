#pragma once

#include <QDialog>
#include <QList>
#include <QString>

#include <QFileSystemWatcher>

#include "tasks/ConcurrentTask.h"

class QPushButton;

struct BlockedMod {
    QString name;
    QString websiteUrl;
    QString hash;
    bool matched;
    QString localPath;
    QString targetFolder;
    bool disabled = false;
    bool move = false;
};

QT_BEGIN_NAMESPACE
namespace Ui {
class BlockedModsDialog;
}
QT_END_NAMESPACE

class BlockedModsDialog : public QDialog {
    Q_OBJECT

   public:
    BlockedModsDialog(QWidget* parent, const QString& title, const QString& text, QList<BlockedMod>& mods, QString hash_type = "sha1");

    ~BlockedModsDialog() override;

   protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

   protected slots:
    void done(int r) override;

   private:
    Ui::BlockedModsDialog* ui;
    QList<BlockedMod>& m_mods;
    QFileSystemWatcher m_watcher;
    shared_qobject_ptr<ConcurrentTask> m_hashingTask;
    QSet<QString> m_pendingHashPaths;
    bool m_rehashPending;
    QString m_hashType;

    void openAll(bool missingOnly);
    void addDownloadFolder();
    void update();
    void directoryChanged(QString path);
    void setupWatch();
    void watchPath(QString path, bool watch_recursive = false);
    void scanPaths();
    void scanPath(QString path, bool start_task);
    void addHashTask(QString path);
    void buildHashTask(QString path);
    void checkMatchHash(QString hash, QString path);
    void validateMatchedMods();
    void runHashTask();
    void hashTaskFinished();

    bool checkValidPath(QString path);
    bool allModsMatched();
};

QDebug operator<<(QDebug debug, const BlockedMod& m);
