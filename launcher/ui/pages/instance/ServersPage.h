#pragma once

#include <QMainWindow>
#include <QString>

#include "BaseInstance.h"
#include "ui/pages/BasePage.h"

#include "settings/Setting.h"

namespace Ui {
class ServersPage;
}

struct Server;
class ServersModel;
class MinecraftInstance;

class ServersPage : public QMainWindow, public BasePage {
    Q_OBJECT

   public:
    explicit ServersPage(BaseInstance* inst, QWidget* parent = 0);
    virtual ~ServersPage();

    void openedImpl() override;
    void closedImpl() override;

    virtual QString displayName() const override { return tr("Servers"); }
    virtual QIcon icon() const override { return QIcon::fromTheme("server"); }
    virtual QString id() const override { return "servers"; }
    virtual QString helpPage() const override { return "Servers-management"; }
    void retranslate() override;

   protected:
    QMenu* createPopupMenu() override;

   private:
    void updateState();
    void scheduleSave();
    bool saveIsScheduled() const;

   private slots:
    void currentChanged(const QModelIndex& current, const QModelIndex& previous);
    void rowsRemoved(const QModelIndex& parent, int first, int last);

    void on_actionAdd_triggered();
    void on_actionRemove_triggered();
    void on_actionMove_Up_triggered();
    void on_actionMove_Down_triggered();
    void on_actionJoin_triggered();
    void on_actionRefresh_triggered();

    void runningStateChanged(bool running);

    void nameEdited(const QString& name);
    void addressEdited(const QString& address);
    void resourceIndexChanged(int index);

    void ShowContextMenu(const QPoint& pos);

   private:  // data
    int currentServer = -1;
    bool m_locked = true;
    Ui::ServersPage* ui = nullptr;
    ServersModel* m_model = nullptr;
    BaseInstance* m_inst = nullptr;

    std::shared_ptr<Setting> m_wide_bar_setting = nullptr;
};
