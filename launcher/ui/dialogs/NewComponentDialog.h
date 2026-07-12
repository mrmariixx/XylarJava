#pragma once

#include <QDialog>

#include <QString>
#include <QStringList>

namespace Ui {
class NewComponentDialog;
}

class NewComponentDialog : public QDialog {
    Q_OBJECT

   public:
    explicit NewComponentDialog(const QString& initialName = QString(), const QString& initialUid = QString(), QWidget* parent = 0);
    virtual ~NewComponentDialog();
    void setBlacklist(QStringList badUids);

    QString name() const;
    QString uid() const;

   private slots:
    void updateDialogState();

   private:
    Ui::NewComponentDialog* ui;

    QString originalPlaceholderText;
    QStringList uidBlacklist;
};
