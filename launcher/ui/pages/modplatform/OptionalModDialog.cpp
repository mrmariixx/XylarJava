#include "OptionalModDialog.h"
#include "ui_OptionalModDialog.h"

OptionalModDialog::OptionalModDialog(QWidget* parent, const QStringList& mods) : QDialog(parent), ui(new Ui::OptionalModDialog)
{
    ui->setupUi(this);
    for (const QString& mod : mods) {
        auto item = new QListWidgetItem(mod, ui->list);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        item->setData(Qt::UserRole, mod);
    }

    connect(ui->selectAllButton, &QPushButton::clicked, ui->list, [this] {
        for (int i = 0; i < ui->list->count(); i++)
            ui->list->item(i)->setCheckState(Qt::Checked);
    });
    connect(ui->clearAllButton, &QPushButton::clicked, ui->list, [this] {
        for (int i = 0; i < ui->list->count(); i++)
            ui->list->item(i)->setCheckState(Qt::Unchecked);
    });
    connect(ui->list, &QListWidget::itemActivated, [](QListWidgetItem* item) {
        if (item->checkState() == Qt::Checked)
            item->setCheckState(Qt::Unchecked);
        else
            item->setCheckState(Qt::Checked);
    });

    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
}

OptionalModDialog::~OptionalModDialog()
{
    delete ui;
}

QStringList OptionalModDialog::getResult()
{
    QStringList result;
    result.reserve(ui->list->count());
    for (int i = 0; i < ui->list->count(); i++) {
        auto item = ui->list->item(i);
        if (item->checkState() == Qt::Checked)
            result.append(item->data(Qt::UserRole).toString());
    }
    return result;
}
