#pragma once

#include <QDialog>
#include <QRegularExpressionValidator>

QT_BEGIN_NAMESPACE
namespace Ui {
class ChooseOfflineNameDialog;
}
QT_END_NAMESPACE

class ChooseOfflineNameDialog final : public QDialog {
    Q_OBJECT

   public:
    explicit ChooseOfflineNameDialog(const QString& message, QWidget* parent = nullptr);
    ~ChooseOfflineNameDialog() override;

    QString getUsername() const;
    void setUsername(const QString& username) const;

   private:
    void updateAcceptAllowed(const QString& username) const;

   protected slots:
    void on_usernameTextBox_textEdited(const QString& newText) const;
    void on_allowInvalidUsernames_checkStateChanged(Qt::CheckState checkState) const;

   private:
    Ui::ChooseOfflineNameDialog* ui;
    QRegularExpressionValidator* m_usernameValidator;
};
