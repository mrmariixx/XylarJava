#include "NewComponentDialog.h"
#include "Application.h"
#include "ui_NewComponentDialog.h"

#include <BaseVersion.h>
#include <InstanceList.h>
#include <icons/IconList.h>
#include <tasks/Task.h>

#include "IconPickerDialog.h"
#include "ProgressDialog.h"
#include "VersionSelectDialog.h"

#include <QFileDialog>
#include <QLayout>
#include <QPushButton>
#include <QValidator>

#include <meta/Index.h>
#include <meta/VersionList.h>

NewComponentDialog::NewComponentDialog(const QString& initialName, const QString& initialUid, QWidget* parent)
    : QDialog(parent), ui(new Ui::NewComponentDialog)
{
    ui->setupUi(this);
    resize(minimumSizeHint());

    ui->nameTextBox->setText(initialName);
    ui->uidTextBox->setText(initialUid);

    connect(ui->nameTextBox, &QLineEdit::textChanged, this, &NewComponentDialog::updateDialogState);
    connect(ui->uidTextBox, &QLineEdit::textChanged, this, &NewComponentDialog::updateDialogState);

    ui->nameTextBox->setFocus();

    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));

    originalPlaceholderText = ui->uidTextBox->placeholderText();
    updateDialogState();
}

NewComponentDialog::~NewComponentDialog()
{
    delete ui;
}

void NewComponentDialog::updateDialogState()
{
    auto protoUid = ui->nameTextBox->text().toLower();
    static const QRegularExpression s_removeChars("[^a-z]");
    protoUid.remove(s_removeChars);
    if (protoUid.isEmpty()) {
        ui->uidTextBox->setPlaceholderText(originalPlaceholderText);
    } else {
        QString suggestedUid = "org.multimc.custom." + protoUid;
        ui->uidTextBox->setPlaceholderText(suggestedUid);
    }
    bool allowOK = !name().isEmpty() && !uid().isEmpty() && !uidBlacklist.contains(uid());
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(allowOK);
}

QString NewComponentDialog::name() const
{
    auto result = ui->nameTextBox->text();
    if (result.size()) {
        return result.trimmed();
    }
    return QString();
}

QString NewComponentDialog::uid() const
{
    auto result = ui->uidTextBox->text();
    if (result.size()) {
        return result.trimmed();
    }
    result = ui->uidTextBox->placeholderText();
    if (result.size() && result != originalPlaceholderText) {
        return result.trimmed();
    }
    return QString();
}

void NewComponentDialog::setBlacklist(QStringList badUids)
{
    uidBlacklist = badUids;
}
