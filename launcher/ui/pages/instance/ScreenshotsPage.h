#pragma once

#include <QMainWindow>

#include "ui/pages/BasePage.h"

#include "settings/Setting.h"

class QIdentityProxyModel;
class QItemSelection;
namespace Ui {
class ScreenshotsPage;
}

class ScreenshotsFSModel;

struct ScreenShot;
class ScreenshotList;
class ImgurAlbumCreation;

class ScreenshotsPage : public QMainWindow, public BasePage {
    Q_OBJECT

   public:
    explicit ScreenshotsPage(QString path, QWidget* parent = 0);
    virtual ~ScreenshotsPage();

    void openedImpl() override;
    void closedImpl() override;

    enum { NothingDone = 0x42 };

    virtual bool eventFilter(QObject*, QEvent*) override;
    virtual QString displayName() const override { return tr("Screenshots"); }
    virtual QIcon icon() const override { return QIcon::fromTheme("screenshots"); }
    virtual QString id() const override { return "screenshots"; }
    virtual QString helpPage() const override { return "Screenshots-management"; }
    virtual bool apply() override { return !m_uploadActive; }
    void retranslate() override;

   protected:
    QMenu* createPopupMenu() override;

   private slots:
    void on_actionUpload_triggered();
    void on_actionCopy_Image_triggered();
    void on_actionCopy_File_s_triggered();
    void on_actionDelete_triggered();
    void on_actionRename_triggered();
    void on_actionView_Folder_triggered();
    void onItemActivated(QModelIndex);
    void onCurrentSelectionChanged(const QItemSelection& selected);
    void ShowContextMenu(const QPoint& pos);

   private:
    Ui::ScreenshotsPage* ui;
    std::shared_ptr<ScreenshotsFSModel> m_model;
    std::shared_ptr<QIdentityProxyModel> m_filterModel;
    QString m_folder;
    bool m_valid = false;
    bool m_uploadActive = false;

    std::shared_ptr<Setting> m_wide_bar_setting = nullptr;
};
