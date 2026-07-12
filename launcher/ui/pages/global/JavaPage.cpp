#include "JavaPage.h"
#include "BuildConfig.h"
#include "JavaCommon.h"
#include "java/JavaInstall.h"
#include "ui/dialogs/CustomMessageBox.h"
#include "ui/java/InstallJavaDialog.h"
#include "ui_JavaPage.h"

#include <QCheckBox>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QStringListModel>
#include <QTabBar>

#include "ui/dialogs/VersionSelectDialog.h"

#include "java/JavaInstallList.h"
#include "java/JavaUtils.h"

#include <FileSystem.h>
#include "Application.h"
#include "settings/SettingsObject.h"

JavaPage::JavaPage(QWidget* parent) : QWidget(parent), ui(new Ui::JavaPage)
{
    ui->setupUi(this);

    if (BuildConfig.JAVA_DOWNLOADER_ENABLED) {
        ui->managedJavaList->initialize(new JavaInstallList(this, true));
        ui->managedJavaList->setResizeOn(2);
        ui->managedJavaList->selectCurrent();
        ui->managedJavaList->setEmptyString(tr("No managed Java versions are installed"));
        ui->managedJavaList->setEmptyErrorString(tr("Couldn't load the managed Java list!"));
    } else
        ui->tabWidget->tabBar()->hide();
}

JavaPage::~JavaPage()
{
    delete ui;
}

void JavaPage::retranslate()
{
    ui->retranslateUi(this);
}

bool JavaPage::apply()
{
    ui->javaSettings->saveSettings();
    JavaCommon::checkJVMArgs(APPLICATION->settings()->get("JvmArgs").toString(), this);
    return true;
}

void JavaPage::on_downloadJavaButton_clicked()
{
    auto jdialog = new Java::InstallDialog({}, nullptr, this);
    jdialog->exec();
    ui->managedJavaList->loadList();
}

void JavaPage::on_removeJavaButton_clicked()
{
    auto version = ui->managedJavaList->selectedVersion();
    auto dcast = std::dynamic_pointer_cast<JavaInstall>(version);
    if (!dcast) {
        return;
    }
    QDir dir(APPLICATION->javaPath());

    auto entries = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (auto& entry : entries) {
        if (dcast->path.startsWith(entry.canonicalFilePath())) {
            auto response = CustomMessageBox::selectable(this, tr("Confirm Deletion"),
                                                         tr("You are about to remove  the Java installation named \"%1\".\n"
                                                            "Are you sure?")
                                                             .arg(entry.fileName()),
                                                         QMessageBox::Warning, QMessageBox::Yes | QMessageBox::No, QMessageBox::No)
                                ->exec();

            if (response == QMessageBox::Yes) {
                FS::deletePath(entry.canonicalFilePath());
                ui->managedJavaList->loadList();
            }
            break;
        }
    }
}
void JavaPage::on_refreshJavaButton_clicked()
{
    ui->managedJavaList->loadList();
}
