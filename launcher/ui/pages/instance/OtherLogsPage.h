#pragma once

#include <QWidget>

#include <Application.h>
#include <QFileSystemWatcher>
#include "LogPage.h"
#include "ui/pages/BasePage.h"

namespace Ui {
class OtherLogsPage;
}

class RecursiveFileSystemWatcher;

class OtherLogsPage : public QWidget, public BasePage {
    Q_OBJECT

   public:
    explicit OtherLogsPage(QString id, QString displayName, QString helpPage, BaseInstance* instance = nullptr, QWidget* parent = 0);
    ~OtherLogsPage();

    QString id() const override { return m_id; }
    QString displayName() const override { return m_displayName; }
    QIcon icon() const override { return QIcon::fromTheme("log"); }
    QString helpPage() const override { return m_helpPage; }
    void retranslate() override;

    void openedImpl() override;
    void closedImpl() override;

   private slots:
    void populateSelectLogBox();
    void on_selectLogBox_currentIndexChanged(int index);
    void on_btnReload_clicked();
    void on_btnPaste_clicked();
    void on_btnCopy_clicked();
    void on_btnDelete_clicked();
    void on_btnClean_clicked();
    void on_btnBottom_clicked();

    void on_trackLogCheckbox_clicked(bool checked);
    void on_wrapCheckbox_clicked(bool checked);
    void on_colorCheckbox_clicked(bool checked);

    void on_findButton_clicked();
    void findActivated();
    void findNextActivated();
    void findPreviousActivated();

   private:
    void reload();
    void modelStateToUI();
    void UIToModelState();
    void setControlsEnabled(bool enabled);

    QStringList getPaths();

   private:
    QString m_id;
    QString m_displayName;
    QString m_helpPage;

    Ui::OtherLogsPage* ui;
    BaseInstance* m_instance;
    /** Path to display log paths relative to. */
    QString m_basePath;
    QStringList m_logSearchPaths;
    QString m_currentFile;
    QFileSystemWatcher m_watcher;

    LogFormatProxyModel* m_proxy;
    LogModel* m_model;
};
