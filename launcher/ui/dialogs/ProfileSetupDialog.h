#pragma once

#include <QDialog>
#include <QIcon>
#include <QNetworkReply>
#include <QTimer>

#include <minecraft/auth/MinecraftAccount.h>
#include "net/Download.h"
#include "net/Upload.h"

namespace Ui {
class ProfileSetupDialog;
}

class ProfileSetupDialog : public QDialog {
    Q_OBJECT
   public:
    explicit ProfileSetupDialog(MinecraftAccountPtr accountToSetup, QWidget* parent = 0);
    ~ProfileSetupDialog();

    enum class NameStatus { NotSet, Pending, Available, Exists, Error } nameStatus = NameStatus::NotSet;

   private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

    void nameEdited(const QString& name);
    void startCheck();

    void checkFinished(QByteArray* response);
    void setupProfileFinished(QByteArray* response);

   protected:
    void scheduleCheck(const QString& name);
    void checkName(const QString& name);
    void setNameStatus(NameStatus status, QString errorString);

    void setupProfile(const QString& profileName);

   private:
    MinecraftAccountPtr m_accountToSetup;
    Ui::ProfileSetupDialog* ui;
    QIcon goodIcon;
    QIcon yellowIcon;
    QIcon badIcon;
    QAction* validityAction = nullptr;

    QString queuedCheck;

    bool isChecking = false;
    bool isWorking = false;
    QString currentCheck;

    QTimer checkStartTimer;

    Net::Download::Ptr m_check_task;
    Net::Upload::Ptr m_profile_task;
};
