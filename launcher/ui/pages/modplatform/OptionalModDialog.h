#pragma once

#include <QAbstractListModel>
#include <QDialog>

namespace Ui {
class OptionalModDialog;
}

class OptionalModDialog : public QDialog {
    Q_OBJECT

   public:
    OptionalModDialog(QWidget* parent, const QStringList& mods);
    ~OptionalModDialog() override;

    QStringList getResult();

   private:
    Ui::OptionalModDialog* ui;
};
