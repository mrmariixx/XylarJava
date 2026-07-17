#pragma once

<<<<<<< HEAD
=======
#include <QTimer>
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
#include <QtWidgets/QDialog>

#include "minecraft/auth/AuthFlow.h"
#include "minecraft/auth/MinecraftAccount.h"

namespace Ui {
class MSALoginDialog;
}

class MSALoginDialog : public QDialog {
    Q_OBJECT

   public:
    ~MSALoginDialog();

    static MinecraftAccountPtr newAccount(QWidget* parent);
    int exec() override;

   private:
    explicit MSALoginDialog(QWidget* parent = 0);

   protected slots:
    void onTaskFailed(QString reason);
<<<<<<< HEAD
    void onFlowStatus(QString status);
    void authorizeWithBrowser(const QUrl& url);
=======
    void onDeviceFlowStatus(QString status);
    void authorizeWithBrowserWithExtra(QString url, QString code, int expiresIn);
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe

   private:
    Ui::MSALoginDialog* ui;
    MinecraftAccountPtr m_account;
<<<<<<< HEAD
    shared_qobject_ptr<AuthFlow> m_login_task;
=======
    shared_qobject_ptr<AuthFlow> m_devicecode_task;

    QUrl m_url;
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
};
