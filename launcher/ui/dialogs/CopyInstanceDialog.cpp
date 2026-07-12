#include <QLayout>
#include <QPushButton>

#include "Application.h"
#include "BuildConfig.h"
#include "CopyInstanceDialog.h"
#include "ui_CopyInstanceDialog.h"

#include "ui/dialogs/IconPickerDialog.h"

#include "BaseInstance.h"
#include "BaseVersion.h"
#include "DesktopServices.h"
#include "FileSystem.h"
#include "InstanceList.h"
#include "icons/IconList.h"

CopyInstanceDialog::CopyInstanceDialog(BaseInstance* original, QWidget* parent)
    : QDialog(parent), ui(new Ui::CopyInstanceDialog), m_original(original)
{
    ui->setupUi(this);
    resize(minimumSizeHint());
    layout()->setSizeConstraint(QLayout::SetFixedSize);

    InstIconKey = original->iconKey();
    ui->iconButton->setIcon(APPLICATION->icons()->getIcon(InstIconKey));
    ui->instNameTextBox->setText(original->name());
    ui->instNameTextBox->setFocus();

    QStringList groups = APPLICATION->instances()->getGroups();
    groups.prepend("");
    ui->groupBox->addItems(groups);
    int index = groups.indexOf(APPLICATION->instances()->getInstanceGroup(m_original->id()));
    if (index == -1)
        index = 0;

    ui->groupBox->setCurrentIndex(index);
    ui->groupBox->lineEdit()->setPlaceholderText(tr("No group"));
    ui->copySavesCheckbox->setChecked(m_selectedOptions.isCopySavesEnabled());
    ui->keepPlaytimeCheckbox->setChecked(m_selectedOptions.isKeepPlaytimeEnabled());
    ui->copyGameOptionsCheckbox->setChecked(m_selectedOptions.isCopyGameOptionsEnabled());
    ui->copyResPacksCheckbox->setChecked(m_selectedOptions.isCopyResourcePacksEnabled());
    ui->copyShaderPacksCheckbox->setChecked(m_selectedOptions.isCopyShaderPacksEnabled());
    ui->copyServersCheckbox->setChecked(m_selectedOptions.isCopyServersEnabled());
    ui->copyModsCheckbox->setChecked(m_selectedOptions.isCopyModsEnabled());
    ui->copyScreenshotsCheckbox->setChecked(m_selectedOptions.isCopyScreenshotsEnabled());

    ui->symbolicLinksCheckbox->setChecked(m_selectedOptions.isUseSymLinksEnabled());
    ui->hardLinksCheckbox->setChecked(m_selectedOptions.isUseHardLinksEnabled());

    ui->recursiveLinkCheckbox->setChecked(m_selectedOptions.isLinkRecursivelyEnabled());
    ui->dontLinkSavesCheckbox->setChecked(m_selectedOptions.isDontLinkSavesEnabled());

    auto detectedFS = FS::statFS(m_original->instanceRoot()).fsType;

    m_cloneSupported = FS::canCloneOnFS(detectedFS);
    m_linkSupported = FS::canLinkOnFS(detectedFS);

    if (m_cloneSupported) {
        ui->cloneSupportedLabel->setText(tr("Reflinks are supported on %1").arg(FS::getFilesystemTypeName(detectedFS)));
    } else {
        ui->cloneSupportedLabel->setText(tr("Reflinks aren't supported on %1").arg(FS::getFilesystemTypeName(detectedFS)));
    }

#if defined(Q_OS_WIN)
    ui->symbolicLinksCheckbox->setIcon(style()->standardIcon(QStyle::SP_VistaShield));
    ui->symbolicLinksCheckbox->setToolTip(tr("Use symbolic links instead of copying files.") + "\n" +
                                          tr("On Windows, symbolic links may require admin permission to create."));
#endif

    updateLinkOptions();
    updateUseCloneCheckbox();

    auto HelpButton = ui->buttonBox->button(QDialogButtonBox::Help);
    connect(HelpButton, &QPushButton::clicked, this, &CopyInstanceDialog::help);
    HelpButton->setText(tr("Help"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
}

CopyInstanceDialog::~CopyInstanceDialog()
{
    delete ui;
}

void CopyInstanceDialog::updateDialogState()
{
    auto allowOK = !instName().isEmpty();
    auto OkButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    if (OkButton->isEnabled() != allowOK) {
        OkButton->setEnabled(allowOK);
    }
}

QString CopyInstanceDialog::instName() const
{
    auto result = ui->instNameTextBox->text().trimmed();
    if (result.size()) {
        return result;
    }
    return QString();
}

QString CopyInstanceDialog::iconKey() const
{
    return InstIconKey;
}

QString CopyInstanceDialog::instGroup() const
{
    return ui->groupBox->currentText();
}

const InstanceCopyPrefs& CopyInstanceDialog::getChosenOptions() const
{
    return m_selectedOptions;
}

void CopyInstanceDialog::help()
{
    DesktopServices::openUrl(QUrl(BuildConfig.HELP_URL.arg("instance-copy")));
}

void CopyInstanceDialog::checkAllCheckboxes(const bool& b)
{
    ui->keepPlaytimeCheckbox->setChecked(b);
    ui->copySavesCheckbox->setChecked(b);
    ui->copyGameOptionsCheckbox->setChecked(b);
    ui->copyResPacksCheckbox->setChecked(b);
    ui->copyShaderPacksCheckbox->setChecked(b);
    ui->copyServersCheckbox->setChecked(b);
    ui->copyModsCheckbox->setChecked(b);
    ui->copyScreenshotsCheckbox->setChecked(b);
}

// Check the "Select all" checkbox if all options are already selected:
void CopyInstanceDialog::updateSelectAllCheckbox()
{
    ui->selectAllCheckbox->blockSignals(true);
    ui->selectAllCheckbox->setChecked(m_selectedOptions.allTrue());
    ui->selectAllCheckbox->blockSignals(false);
}

void CopyInstanceDialog::updateUseCloneCheckbox()
{
    ui->useCloneCheckbox->setEnabled(m_cloneSupported && !ui->symbolicLinksCheckbox->isChecked() && !ui->hardLinksCheckbox->isChecked());
    ui->useCloneCheckbox->setChecked(m_cloneSupported && m_selectedOptions.isUseCloneEnabled() && !ui->symbolicLinksCheckbox->isChecked() &&
                                     !ui->hardLinksCheckbox->isChecked());
}

void CopyInstanceDialog::updateLinkOptions()
{
    ui->symbolicLinksCheckbox->setEnabled(m_linkSupported && !ui->hardLinksCheckbox->isChecked() && !ui->useCloneCheckbox->isChecked());
    ui->hardLinksCheckbox->setEnabled(m_linkSupported && !ui->symbolicLinksCheckbox->isChecked() && !ui->useCloneCheckbox->isChecked());

    ui->symbolicLinksCheckbox->setChecked(m_linkSupported && m_selectedOptions.isUseSymLinksEnabled() &&
                                          !ui->useCloneCheckbox->isChecked());
    ui->hardLinksCheckbox->setChecked(m_linkSupported && m_selectedOptions.isUseHardLinksEnabled() && !ui->useCloneCheckbox->isChecked());

    bool linksInUse = (ui->symbolicLinksCheckbox->isChecked() || ui->hardLinksCheckbox->isChecked());
    ui->recursiveLinkCheckbox->setEnabled(m_linkSupported && linksInUse && !ui->hardLinksCheckbox->isChecked());
    ui->dontLinkSavesCheckbox->setEnabled(m_linkSupported && linksInUse);
    ui->recursiveLinkCheckbox->setChecked(m_linkSupported && linksInUse && m_selectedOptions.isLinkRecursivelyEnabled());
    ui->dontLinkSavesCheckbox->setChecked(m_linkSupported && linksInUse && m_selectedOptions.isDontLinkSavesEnabled());

#if defined(Q_OS_WIN)
    auto OkButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    OkButton->setIcon(m_selectedOptions.isUseSymLinksEnabled() ? style()->standardIcon(QStyle::SP_VistaShield) : QIcon());
#endif
}

void CopyInstanceDialog::on_iconButton_clicked()
{
    IconPickerDialog dlg(this);
    dlg.execWithSelection(InstIconKey);

    if (dlg.result() == QDialog::Accepted) {
        InstIconKey = dlg.selectedIconKey;
        ui->iconButton->setIcon(APPLICATION->icons()->getIcon(InstIconKey));
    }
}

void CopyInstanceDialog::on_instNameTextBox_textChanged([[maybe_unused]] const QString& arg1)
{
    updateDialogState();
}

void CopyInstanceDialog::on_selectAllCheckbox_stateChanged(int state)
{
    bool checked;
    checked = (state == Qt::Checked);
    checkAllCheckboxes(checked);
}

void CopyInstanceDialog::on_copySavesCheckbox_stateChanged(int state)
{
    m_selectedOptions.enableCopySaves(state == Qt::Checked);
    ui->dontLinkSavesCheckbox->setChecked((state == Qt::Checked) && ui->dontLinkSavesCheckbox->isChecked());
    updateSelectAllCheckbox();
}

void CopyInstanceDialog::on_keepPlaytimeCheckbox_stateChanged(int state)
{
    m_selectedOptions.enableKeepPlaytime(state == Qt::Checked);
    updateSelectAllCheckbox();
}

void CopyInstanceDialog::on_copyGameOptionsCheckbox_stateChanged(int state)
{
    m_selectedOptions.enableCopyGameOptions(state == Qt::Checked);
    updateSelectAllCheckbox();
}

void CopyInstanceDialog::on_copyResPacksCheckbox_stateChanged(int state)
{
    m_selectedOptions.enableCopyResourcePacks(state == Qt::Checked);
    updateSelectAllCheckbox();
}

void CopyInstanceDialog::on_copyShaderPacksCheckbox_stateChanged(int state)
{
    m_selectedOptions.enableCopyShaderPacks(state == Qt::Checked);
    updateSelectAllCheckbox();
}

void CopyInstanceDialog::on_copyServersCheckbox_stateChanged(int state)
{
    m_selectedOptions.enableCopyServers(state == Qt::Checked);
    updateSelectAllCheckbox();
}

void CopyInstanceDialog::on_copyModsCheckbox_stateChanged(int state)
{
    m_selectedOptions.enableCopyMods(state == Qt::Checked);
    updateSelectAllCheckbox();
}

void CopyInstanceDialog::on_copyScreenshotsCheckbox_stateChanged(int state)
{
    m_selectedOptions.enableCopyScreenshots(state == Qt::Checked);
    updateSelectAllCheckbox();
}

void CopyInstanceDialog::on_symbolicLinksCheckbox_stateChanged(int state)
{
    m_selectedOptions.enableUseSymLinks(state == Qt::Checked);
    updateUseCloneCheckbox();
    updateLinkOptions();
}

void CopyInstanceDialog::on_hardLinksCheckbox_stateChanged(int state)
{
    m_selectedOptions.enableUseHardLinks(state == Qt::Checked);
    if (state == Qt::Checked && !ui->recursiveLinkCheckbox->isChecked()) {
        ui->recursiveLinkCheckbox->setChecked(true);
    }
    updateUseCloneCheckbox();
    updateLinkOptions();
}

void CopyInstanceDialog::on_recursiveLinkCheckbox_stateChanged(int state)
{
    m_selectedOptions.enableLinkRecursively(state == Qt::Checked);
    updateLinkOptions();
}

void CopyInstanceDialog::on_dontLinkSavesCheckbox_stateChanged(int state)
{
    m_selectedOptions.enableDontLinkSaves(state == Qt::Checked);
}

void CopyInstanceDialog::on_useCloneCheckbox_stateChanged(int state)
{
    m_selectedOptions.enableUseClone(m_cloneSupported && (state == Qt::Checked));
    updateUseCloneCheckbox();
    updateLinkOptions();
}
