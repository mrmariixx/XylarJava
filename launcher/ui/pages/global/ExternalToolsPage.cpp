#include "ExternalToolsPage.h"
#include "ui_ExternalToolsPage.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QTabBar>

#include <FileSystem.h>
#include <tools/MCEditTool.h>
#include "Application.h"
#include "settings/SettingsObject.h"
#include "tools/BaseProfiler.h"

ExternalToolsPage::ExternalToolsPage(QWidget* parent) : QWidget(parent), ui(new Ui::ExternalToolsPage)
{
    ui->setupUi(this);

    ui->jsonEditorTextBox->setClearButtonEnabled(true);

    ui->mceditLink->setOpenExternalLinks(true);
    ui->jvisualvmLink->setOpenExternalLinks(true);
    ui->jprofilerLink->setOpenExternalLinks(true);
    loadSettings();
}

ExternalToolsPage::~ExternalToolsPage()
{
    delete ui;
}

void ExternalToolsPage::loadSettings()
{
    auto s = APPLICATION->settings();
    ui->jprofilerPathEdit->setText(s->get("JProfilerPath").toString());
    ui->jvisualvmPathEdit->setText(s->get("JVisualVMPath").toString());
    ui->mceditPathEdit->setText(s->get("MCEditPath").toString());

    // Editors
    ui->jsonEditorTextBox->setText(s->get("JsonEditor").toString());
}
void ExternalToolsPage::applySettings()
{
    auto s = APPLICATION->settings();

    s->set("JProfilerPath", ui->jprofilerPathEdit->text());
    s->set("JVisualVMPath", ui->jvisualvmPathEdit->text());
    s->set("MCEditPath", ui->mceditPathEdit->text());

    // Editors
    QString jsonEditor = ui->jsonEditorTextBox->text();
    if (!jsonEditor.isEmpty() && (!QFileInfo(jsonEditor).exists() || !QFileInfo(jsonEditor).isExecutable())) {
        QString found = QStandardPaths::findExecutable(jsonEditor);
        if (!found.isEmpty()) {
            jsonEditor = found;
        }
    }
    s->set("JsonEditor", jsonEditor);
}

void ExternalToolsPage::on_jprofilerPathBtn_clicked()
{
    QString raw_dir = ui->jprofilerPathEdit->text();
    QString error;
    do {
        raw_dir = QFileDialog::getExistingDirectory(this, tr("JProfiler Folder"), raw_dir);
        if (raw_dir.isEmpty()) {
            break;
        }
        QString cooked_dir = FS::NormalizePath(raw_dir);
        if (!APPLICATION->profilers()["jprofiler"]->check(cooked_dir, &error)) {
            QMessageBox::critical(this, tr("Error"), tr("Error while checking JProfiler install:\n%1").arg(error));
            continue;
        } else {
            ui->jprofilerPathEdit->setText(cooked_dir);
            break;
        }
    } while (1);
}
void ExternalToolsPage::on_jprofilerCheckBtn_clicked()
{
    QString error;
    if (!APPLICATION->profilers()["jprofiler"]->check(ui->jprofilerPathEdit->text(), &error)) {
        QMessageBox::critical(this, tr("Error"), tr("Error while checking JProfiler install:\n%1").arg(error));
    } else {
        QMessageBox::information(this, tr("OK"), tr("JProfiler setup seems to be OK"));
    }
}

void ExternalToolsPage::on_jvisualvmPathBtn_clicked()
{
    QString raw_dir = ui->jvisualvmPathEdit->text();
    QString error;
    do {
        raw_dir = QFileDialog::getOpenFileName(this, tr("VisualVM Executable"), raw_dir);
        if (raw_dir.isEmpty()) {
            break;
        }
        QString cooked_dir = FS::NormalizePath(raw_dir);
        if (!APPLICATION->profilers()["jvisualvm"]->check(cooked_dir, &error)) {
            QMessageBox::critical(this, tr("Error"), tr("Error while checking VisualVM install:\n%1").arg(error));
            continue;
        } else {
            ui->jvisualvmPathEdit->setText(cooked_dir);
            break;
        }
    } while (1);
}
void ExternalToolsPage::on_jvisualvmCheckBtn_clicked()
{
    QString error;
    if (!APPLICATION->profilers()["jvisualvm"]->check(ui->jvisualvmPathEdit->text(), &error)) {
        QMessageBox::critical(this, tr("Error"), tr("Error while checking VisualVM install:\n%1").arg(error));
    } else {
        QMessageBox::information(this, tr("OK"), tr("VisualVM setup seems to be OK"));
    }
}

void ExternalToolsPage::on_mceditPathBtn_clicked()
{
    QString raw_dir = ui->mceditPathEdit->text();
    QString error;
    do {
#ifdef Q_OS_MACOS
        raw_dir = QFileDialog::getOpenFileName(this, tr("MCEdit Application"), raw_dir);
#else
        raw_dir = QFileDialog::getExistingDirectory(this, tr("MCEdit Folder"), raw_dir);
#endif
        if (raw_dir.isEmpty()) {
            break;
        }
        QString cooked_dir = FS::NormalizePath(raw_dir);
        if (!APPLICATION->mcedit()->check(cooked_dir, error)) {
            QMessageBox::critical(this, tr("Error"), tr("Error while checking MCEdit install:\n%1").arg(error));
            continue;
        } else {
            ui->mceditPathEdit->setText(cooked_dir);
            break;
        }
    } while (1);
}
void ExternalToolsPage::on_mceditCheckBtn_clicked()
{
    QString error;
    if (!APPLICATION->mcedit()->check(ui->mceditPathEdit->text(), error)) {
        QMessageBox::critical(this, tr("Error"), tr("Error while checking MCEdit install:\n%1").arg(error));
    } else {
        QMessageBox::information(this, tr("OK"), tr("MCEdit setup seems to be OK"));
    }
}

void ExternalToolsPage::on_jsonEditorBrowseBtn_clicked()
{
    QString raw_file = QFileDialog::getOpenFileName(this, tr("Text Editor"),
                                                    ui->jsonEditorTextBox->text().isEmpty()
#if defined(Q_OS_LINUX)
                                                        ? QString("/usr/bin")
#else
                                                        ? QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation).first()
#endif
                                                        : ui->jsonEditorTextBox->text());

    if (raw_file.isEmpty()) {
        return;
    }
    QString cooked_file = FS::NormalizePath(raw_file);

    // it has to exist and be an executable
    if (QFileInfo(cooked_file).exists() && QFileInfo(cooked_file).isExecutable()) {
        ui->jsonEditorTextBox->setText(cooked_file);
    } else {
        QMessageBox::warning(this, tr("Invalid"), tr("The file chosen does not seem to be an executable"));
    }
}

bool ExternalToolsPage::apply()
{
    applySettings();
    return true;
}

void ExternalToolsPage::retranslate()
{
    ui->retranslateUi(this);
}
