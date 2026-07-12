#include "ImportPage.h"

#include "ui/dialogs/ProgressDialog.h"
#include "ui_ImportPage.h"

#include <QFileDialog>
#include <QMimeDatabase>
#include <QValidator>
#include <utility>

#include "ui/dialogs/CustomMessageBox.h"
#include "ui/dialogs/NewInstanceDialog.h"

#include "modplatform/flame/FlameAPI.h"

#include "Json.h"

#include "InstanceImportTask.h"
#include "net/NetJob.h"

class UrlValidator : public QValidator {
   public:
    using QValidator::QValidator;

    State validate(QString& in, [[maybe_unused]] int& pos) const
    {
        const QUrl url(in);
        if (url.isValid() && !url.isRelative() && !url.isEmpty()) {
            return Acceptable;
        } else if (QFile::exists(in)) {
            return Acceptable;
        } else {
            return Intermediate;
        }
    }
};

ImportPage::ImportPage(NewInstanceDialog* dialog, QWidget* parent) : QWidget(parent), ui(new Ui::ImportPage), dialog(dialog)
{
    ui->setupUi(this);
    ui->modpackEdit->setValidator(new UrlValidator(ui->modpackEdit));
    connect(ui->modpackEdit, &QLineEdit::textChanged, this, &ImportPage::updateState);
}

ImportPage::~ImportPage()
{
    delete ui;
}

bool ImportPage::shouldDisplay() const
{
    return true;
}

void ImportPage::retranslate()
{
    ui->retranslateUi(this);
}

void ImportPage::openedImpl()
{
    updateState();
}

void ImportPage::updateState()
{
    if (!isOpened) {
        return;
    }
    if (ui->modpackEdit->hasAcceptableInput()) {
        QString input = ui->modpackEdit->text().trimmed();
        auto url = QUrl::fromUserInput(input);
        if (url.isLocalFile()) {
            // FIXME: actually do some validation of what's inside here... this is fake AF
            QFileInfo fi(input);

            // Allow non-latin people to use ZIP files!
            bool isZip = QMimeDatabase().mimeTypeForUrl(url).suffixes().contains("zip");
            // mrpack is a modrinth pack
            bool isMRPack = fi.suffix() == "mrpack";

            if (fi.exists() && (isZip || isMRPack)) {
                auto extra_info = QMap(m_extra_info);
                qDebug() << "Pack Extra Info" << extra_info << m_extra_info;
                dialog->setSuggestedPack(fi.completeBaseName(), new InstanceImportTask(url, this, std::move(extra_info)));
                dialog->setSuggestedIcon("default");
            }
        } else if (url.scheme() == "curseforge") {
            // need to find the download link for the modpack
            // format of url curseforge://install?addonId=IDHERE&fileId=IDHERE
            QUrlQuery query(url);
            if (query.allQueryItemValues("addonId").isEmpty() || query.allQueryItemValues("fileId").isEmpty()) {
                qDebug() << "Invalid curseforge link:" << url;
                return;
            }
            auto addonId = query.allQueryItemValues("addonId")[0];
            auto fileId = query.allQueryItemValues("fileId")[0];

            auto api = FlameAPI();
            auto [job, array] = api.getFile(addonId, fileId);

            connect(job.get(), &NetJob::failed, this,
                    [this](QString reason) { CustomMessageBox::selectable(this, tr("Error"), reason, QMessageBox::Critical)->show(); });
            connect(job.get(), &NetJob::succeeded, this, [this, array, addonId, fileId] {
                qDebug() << "Returned CFURL Json:\n" << array->toStdString().c_str();
                auto doc = Json::requireDocument(*array);
                auto data = doc.object()["data"].toObject();
                // No way to find out if it's a mod or a modpack before here
                // And also we need to check if it ends with .zip, instead of any better way
                auto fileName = data["fileName"].toString();
                if (fileName.endsWith(".zip")) {
                    // Have to use ensureString then use QUrl to get proper url encoding
                    auto dl_url = QUrl(data["downloadUrl"].toString(""));
                    if (!dl_url.isValid()) {
                        CustomMessageBox::selectable(
                            this, tr("Error"),
                            tr("The modpack %1 is blocked for third-parties! Please download it manually.").arg(fileName),
                            QMessageBox::Critical)
                            ->show();
                        return;
                    }

                    QFileInfo dl_file(dl_url.fileName());
                    QString pack_name = data["displayName"].toString(dl_file.completeBaseName());

                    QMap<QString, QString> extra_info;
                    extra_info.insert("pack_id", addonId);
                    extra_info.insert("pack_version_id", fileId);

                    dialog->setSuggestedPack(pack_name, new InstanceImportTask(dl_url, this, std::move(extra_info)));
                    dialog->setSuggestedIcon("default");

                } else {
                    CustomMessageBox::selectable(this, tr("Error"), tr("This url isn't a valid modpack !"), QMessageBox::Critical)->show();
                }
            });
            ProgressDialog dlUrlDialod(this);
            dlUrlDialod.setSkipButton(true, tr("Abort"));
            dlUrlDialod.execWithTask(job.get());
            return;
        } else {
            if (input.endsWith("?client=y")) {
                input.chop(9);
                input.append("/file");
                url = QUrl::fromUserInput(input);
            }
            // hook, line and sinker.
            QFileInfo fi(url.fileName());
            auto extra_info = QMap(m_extra_info);
            dialog->setSuggestedPack(fi.completeBaseName(), new InstanceImportTask(url, this, std::move(extra_info)));
            dialog->setSuggestedIcon("default");
        }
    } else {
        dialog->setSuggestedPack();
    }
}

void ImportPage::setUrl(const QString& url)
{
    ui->modpackEdit->setText(url);
    updateState();
}

void ImportPage::setExtraInfo(const QMap<QString, QString>& extra_info)
{
    m_extra_info = extra_info;
    updateState();
}

void ImportPage::on_modpackBtn_clicked()
{
    const QMimeType zip = QMimeDatabase().mimeTypeForName("application/zip");
    auto filter = tr("Supported files") + QString(" (%1 *.mrpack)").arg(zip.globPatterns().join(" "));
    filter += ";;" + zip.filterString();
    //: Option for filtering for *.mrpack files when importing
    filter += ";;" + tr("Modrinth pack") + " (*.mrpack)";
    const QUrl url = QFileDialog::getOpenFileUrl(this, tr("Choose modpack"), modpackUrl(), filter);
    if (url.isValid()) {
        if (url.isLocalFile()) {
            ui->modpackEdit->setText(url.toLocalFile());
        } else {
            ui->modpackEdit->setText(url.toString());
        }
    }
}

QUrl ImportPage::modpackUrl() const
{
    const QUrl url(ui->modpackEdit->text());
    if (url.isValid() && !url.isRelative() && !url.host().isEmpty()) {
        return url;
    } else {
        return QUrl::fromLocalFile(ui->modpackEdit->text());
    }
}
