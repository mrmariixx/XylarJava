#pragma once

#include <QDialog>

namespace Ui {
class UpdateAvailableDialog;
}

class UpdateAvailableDialog : public QDialog {
    Q_OBJECT

   public:
    enum ResultCode {
        Install = 10,
        DontInstall = 11,
        Skip = 12,
    };

    explicit UpdateAvailableDialog(const QString& currentVersion,
                                   const QString& availableVersion,
                                   const QString& releaseNotes,
                                   QWidget* parent = 0);
    ~UpdateAvailableDialog() = default;

   private:
    Ui::UpdateAvailableDialog* ui;
};
