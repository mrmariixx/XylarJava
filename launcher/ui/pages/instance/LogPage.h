#pragma once

#include <QIdentityProxyModel>
#include <QWidget>

#include "BaseInstance.h"
#include "launch/LaunchTask.h"
#include "ui/pages/BasePage.h"

namespace Ui {
class LogPage;
}
class QTextCharFormat;

class LogFormatProxyModel : public QIdentityProxyModel {
   public:
    LogFormatProxyModel(QObject* parent = nullptr) : QIdentityProxyModel(parent) {}
    QVariant data(const QModelIndex& index, int role) const override;
    QFont getFont() const { return m_font; }
    void setFont(QFont font) { m_font = font; }
    QModelIndex find(const QModelIndex& start, const QString& value, bool reverse) const;

   private:
    QFont m_font;
};

class LogPage : public QWidget, public BasePage {
    Q_OBJECT

   public:
    explicit LogPage(BaseInstance* instance, QWidget* parent = 0);
    virtual ~LogPage();
    virtual QString displayName() const override { return tr("Minecraft Log"); }
    virtual QIcon icon() const override { return QIcon::fromTheme("log"); }
    virtual QString id() const override { return "console"; }
    virtual bool apply() override;
    virtual QString helpPage() const override { return "Minecraft-Logs"; }
    virtual bool shouldDisplay() const override;
    void retranslate() override;

   private slots:
    void on_btnPaste_clicked();
    void on_btnCopy_clicked();
    void on_btnClear_clicked();
    void on_btnBottom_clicked();

    void on_trackLogCheckbox_clicked(bool checked);
    void on_wrapCheckbox_clicked(bool checked);
    void on_colorCheckbox_clicked(bool checked);

    void on_findButton_clicked();
    void findActivated();
    void findNextActivated();
    void findPreviousActivated();

    void onInstanceLaunchTaskChanged(LaunchTask* proc);

   private:
    void modelStateToUI();
    void UIToModelState();
    void setInstanceLaunchTaskChanged(LaunchTask* proc, bool initial);

   private:
    Ui::LogPage* ui;
    BaseInstance* m_instance;
    LaunchTask* m_process;

    LogFormatProxyModel* m_proxy;
    shared_qobject_ptr<LogModel> m_model;
};
