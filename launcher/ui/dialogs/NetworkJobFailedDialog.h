#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class NetworkJobFailedDialog;
}
QT_END_NAMESPACE

class NetworkJobFailedDialog : public QDialog {
    Q_OBJECT

   public:
    explicit NetworkJobFailedDialog(const QString& jobName, int attempts, int requests, int failed, QWidget* parent = nullptr);
    ~NetworkJobFailedDialog() override;

    void addFailedRequest(const QUrl& url, QString error) const;

   private slots:
    void copyUrl() const;

   private:
    Ui::NetworkJobFailedDialog* m_ui;
};
