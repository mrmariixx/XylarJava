#include "UpdateAvailableDialog.h"
#include <QPushButton>
#include "BuildConfig.h"
#include "Markdown.h"
#include "StringUtils.h"
#include "ui_UpdateAvailableDialog.h"

UpdateAvailableDialog::UpdateAvailableDialog(const QString& currentVersion,
                                             const QString& availableVersion,
                                             const QString& releaseNotes,
                                             QWidget* parent)
    : QDialog(parent), ui(new Ui::UpdateAvailableDialog)
{
    ui->setupUi(this);

    QString launcherName = BuildConfig.LAUNCHER_DISPLAYNAME;

    ui->headerLabel->setText(tr("A new version of %1 is available!").arg(launcherName));
    ui->versionAvailableLabel->setText(
        tr("Version %1 is now available - you have %2 . Would you like to download it now?").arg(availableVersion).arg(currentVersion));
    ui->icon->setPixmap(QIcon::fromTheme("checkupdate").pixmap(64));

    auto releaseNotesHtml = markdownToHTML(releaseNotes);
    ui->releaseNotes->setHtml(StringUtils::htmlListPatch(releaseNotesHtml));
    ui->releaseNotes->setOpenExternalLinks(true);

    connect(ui->skipButton, &QPushButton::clicked, this, [this]() {
        setResult(ResultCode::Skip);
        done(ResultCode::Skip);
    });

    connect(ui->delayButton, &QPushButton::clicked, this, [this]() {
        setResult(ResultCode::DontInstall);
        done(ResultCode::DontInstall);
    });

    connect(ui->installButton, &QPushButton::clicked, this, [this]() {
        setResult(ResultCode::Install);
        done(ResultCode::Install);
    });
}
