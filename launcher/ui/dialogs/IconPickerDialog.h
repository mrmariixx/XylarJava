#pragma once
#include <QDialog>
#include <QItemSelection>
#include <QLineEdit>
#include <QSortFilterProxyModel>

namespace Ui {
class IconPickerDialog;
}

class IconPickerDialog : public QDialog {
    Q_OBJECT

   public:
    explicit IconPickerDialog(QWidget* parent = 0);
    ~IconPickerDialog();
    int execWithSelection(QString selection);
    QString selectedIconKey;

   protected:
    virtual bool eventFilter(QObject*, QEvent*);

   private:
    Ui::IconPickerDialog* ui;
    QPushButton* buttonRemove;
    QLineEdit* searchBar;
    QSortFilterProxyModel* proxyModel;

   private slots:
    void selectionChanged(QItemSelection, QItemSelection);
    void activated(QModelIndex);
    void delayed_scroll(QModelIndex);
    void addNewIcon();
    void removeSelectedIcon();
    void openFolder();
    void filterIcons(const QString& text);
};
