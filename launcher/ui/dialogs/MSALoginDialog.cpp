#include "MSALoginDialog.h"
#include "Application.h"
<<<<<<< HEAD
=======
#include "settings/SettingsObject.h"
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe

#include "ui_MSALoginDialog.h"

#include "DesktopServices.h"
#include "minecraft/auth/AuthFlow.h"

#include <QApplication>
#include <QClipboard>
<<<<<<< HEAD
#include <QMessageBox>
#include <QUrl>
#include <QtWidgets/QPushButton>

=======
#include <QColor>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QSize>
#include <QUrl>
#include <QtWidgets/QPushButton>

#include "qrencode.h"

>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
MSALoginDialog::MSALoginDialog(QWidget* parent) : QDialog(parent), ui(new Ui::MSALoginDialog)
{
    ui->setupUi(this);

<<<<<<< HEAD
    // Browser OAuth only — hide device-code / microsoft.com/link UI
    ui->stackedWidget->setCurrentIndex(0);
    if (auto* page = ui->stackedWidget->widget(1)) {
        ui->stackedWidget->removeWidget(page);
        page->deleteLater();
    }

    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    ui->loadingLabel->setText(tr("Sign in with Microsoft"));
    ui->status->setText(tr("Opening your browser..."));
=======
    QFont font;
    font.setPixelSize(ui->code->fontInfo().pixelSize());
    font.setFamily(APPLICATION->settings()->get("ConsoleFont").toString());
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    ui->code->setFont(font);

    connect(ui->copyCode, &QPushButton::clicked, this, [this] { QApplication::clipboard()->setText(ui->code->text()); });
    connect(ui->openLinkButton, &QPushButton::clicked, this, [this] {
        if (m_url.isValid()) {
            if (!DesktopServices::openUrl(m_url)) {
                QApplication::clipboard()->setText(m_url.toString());
            }
        }
    });

    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
}

int MSALoginDialog::exec()
{
    if (APPLICATION->getMSAClientID().isEmpty()) {
<<<<<<< HEAD
        QMessageBox::warning(this, tr("Microsoft login not configured"),
                             tr("No Azure client ID was found. Create auth-settings.ini next to the launcher executable."));
=======
        QMessageBox::warning(
            this, tr("Microsoft login not configured"),
            tr("No Azure client ID was found. Create auth-settings.ini next to the launcher executable."));
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
        return QDialog::Rejected;
    }

    m_account = MinecraftAccount::createBlankMSA();
<<<<<<< HEAD
    // Browser authorization-code flow (NOT device code / microsoft.com/link)
    m_login_task = m_account->login(false);
    connect(m_login_task.get(), &Task::failed, this, &MSALoginDialog::onTaskFailed);
    connect(m_login_task.get(), &Task::succeeded, this, &QDialog::accept);
    connect(m_login_task.get(), &Task::aborted, this, &MSALoginDialog::reject);
    connect(m_login_task.get(), &Task::status, this, &MSALoginDialog::onFlowStatus);
    connect(m_login_task.get(), &AuthFlow::authorizeWithBrowser, this, &MSALoginDialog::authorizeWithBrowser);
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, m_login_task.get(), &Task::abort);
    QMetaObject::invokeMethod(m_login_task.get(), &Task::start, Qt::QueuedConnection);
=======
    m_devicecode_task = m_account->login(true);
    connect(m_devicecode_task.get(), &Task::failed, this, &MSALoginDialog::onTaskFailed);
    connect(m_devicecode_task.get(), &Task::succeeded, this, &QDialog::accept);
    connect(m_devicecode_task.get(), &Task::aborted, this, &MSALoginDialog::reject);
    connect(m_devicecode_task.get(), &Task::status, this, &MSALoginDialog::onDeviceFlowStatus);
    connect(m_devicecode_task.get(), &AuthFlow::authorizeWithBrowserWithExtra, this, &MSALoginDialog::authorizeWithBrowserWithExtra);
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, m_devicecode_task.get(), &Task::abort);
    QMetaObject::invokeMethod(m_devicecode_task.get(), &Task::start, Qt::QueuedConnection);
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe

    return QDialog::exec();
}

MSALoginDialog::~MSALoginDialog()
{
    delete ui;
}

void MSALoginDialog::onTaskFailed(QString reason)
{
<<<<<<< HEAD
    m_login_task->disconnect();
=======
    m_devicecode_task->disconnect();
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
    ui->stackedWidget->setCurrentIndex(0);
    auto lines = reason.split('\n');
    QString processed;
    for (auto line : lines) {
        if (line.size()) {
<<<<<<< HEAD
            processed += "<font color='red'>" + line.toHtmlEscaped() + "</font><br />";
=======
            processed += "<font color='red'>" + line + "</font><br />";
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
        } else {
            processed += "<br />";
        }
    }
    ui->status->setText(processed);
<<<<<<< HEAD
    if (m_login_task) {
        ui->loadingLabel->setText(m_login_task->getStatus());
    }
    disconnect(ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, m_login_task.get(), &Task::abort);
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &MSALoginDialog::reject);
}

void MSALoginDialog::authorizeWithBrowser(const QUrl& url)
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->loadingLabel->setText(tr("Complete sign-in in your browser"));
    ui->status->setText(tr("A browser window was opened. Finish Microsoft login there, then return here.\n\n"
                           "If nothing opened, visit:\n%1")
                            .arg(url.toString()));
    if (!DesktopServices::openUrl(url)) {
        QApplication::clipboard()->setText(url.toString());
        ui->status->setText(tr("Could not open the browser. The login URL was copied to the clipboard:\n%1").arg(url.toString()));
    }
}

void MSALoginDialog::onFlowStatus(QString status)
{
    ui->stackedWidget->setCurrentIndex(0);
=======
    if (m_devicecode_task) {
        ui->loadingLabel->setText(m_devicecode_task->getStatus());
    }
    disconnect(ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, m_devicecode_task.get(), &Task::abort);
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &MSALoginDialog::reject);
}

void paintQR(QPainter& painter, const QSize canvasSize, const QString& data, QColor fg)
{
    const auto* qr = QRcode_encodeString(data.toUtf8().constData(), 0, QRecLevel::QR_ECLEVEL_M, QRencodeMode::QR_MODE_8, 1);
    if (!qr) {
        qWarning() << "Unable to encode" << data << "as QR code";
        return;
    }

    painter.setPen(Qt::NoPen);
    painter.setBrush(fg);

    const auto qrSize = qr->width;
    const auto canvasWidth = canvasSize.width();
    const auto canvasHeight = canvasSize.height();
    const auto scale = 0.8 * std::min(canvasWidth / qrSize, canvasHeight / qrSize);

    const auto offsetX = (canvasWidth - qrSize * scale) / 2;
    const auto offsetY = (canvasHeight - qrSize * scale) / 2;

    for (int y = 0; y < qrSize; y++) {
        for (int x = 0; x < qrSize; x++) {
            auto shouldFillIn = qr->data[y * qrSize + x] & 1;
            if (shouldFillIn) {
                QRectF r(offsetX + x * scale, offsetY + y * scale, scale, scale);
                painter.drawRects(&r, 1);
            }
        }
    }
}

void MSALoginDialog::authorizeWithBrowserWithExtra(QString url, QString code, [[maybe_unused]] int expiresIn)
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->stackedWidget->adjustSize();
    ui->stackedWidget->updateGeometry();
    this->adjustSize();

    const auto linkString = QString("<a href=\"%1\">%2</a>").arg(url, url);
    if (url == "https://www.microsoft.com/link" && !code.isEmpty()) {
        url += QString("?otc=%1").arg(code);
    }
    m_url = QUrl(url);
    ui->code->setText(code);

    auto size = QSize(150, 150);
    QPixmap pixmap(size);
    pixmap.fill(Qt::white);

    QPainter painter(&pixmap);
    paintQR(painter, size, url, Qt::black);

    ui->qr->setPixmap(pixmap);

    ui->qrMessage->setText(tr("Open %1 or scan the QR code and enter the above code if needed.").arg(linkString));
}

void MSALoginDialog::onDeviceFlowStatus(QString status)
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->stackedWidget->adjustSize();
    ui->stackedWidget->updateGeometry();
    this->adjustSize();
>>>>>>> bbd42f92ed29e2e874cb4182999b18155dd83efe
    ui->status->setText(status);
}

MinecraftAccountPtr MSALoginDialog::newAccount(QWidget* parent)
{
    MSALoginDialog dlg(parent);
    if (dlg.exec() == QDialog::Accepted) {
        return dlg.m_account;
    }
    return nullptr;
}
