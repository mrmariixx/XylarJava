#include "ProxyPage.h"
#include "ui_ProxyPage.h"

#include <QButtonGroup>
#include <QTabBar>

#include "Application.h"
#include "settings/SettingsObject.h"

ProxyPage::ProxyPage(QWidget* parent) : QWidget(parent), ui(new Ui::ProxyPage)
{
    ui->setupUi(this);
    loadSettings();
    updateCheckboxStuff();

    connect(ui->proxyGroup, &QButtonGroup::buttonClicked, this, &ProxyPage::proxyGroupChanged);
}

ProxyPage::~ProxyPage()
{
    delete ui;
}

bool ProxyPage::apply()
{
    applySettings();
    return true;
}

void ProxyPage::updateCheckboxStuff()
{
    bool enableEditing = ui->proxyHTTPBtn->isChecked() || ui->proxySOCKS5Btn->isChecked();
    ui->proxyAddrBox->setEnabled(enableEditing);
    ui->proxyAuthBox->setEnabled(enableEditing);
}

void ProxyPage::proxyGroupChanged([[maybe_unused]] QAbstractButton* button)
{
    updateCheckboxStuff();
}

void ProxyPage::applySettings()
{
    auto s = APPLICATION->settings();

    // Proxy
    QString proxyType = "None";
    if (ui->proxyDefaultBtn->isChecked())
        proxyType = "Default";
    else if (ui->proxyNoneBtn->isChecked())
        proxyType = "None";
    else if (ui->proxySOCKS5Btn->isChecked())
        proxyType = "SOCKS5";
    else if (ui->proxyHTTPBtn->isChecked())
        proxyType = "HTTP";

    s->set("ProxyType", proxyType);
    s->set("ProxyAddr", ui->proxyAddrEdit->text());
    s->set("ProxyPort", ui->proxyPortEdit->value());
    s->set("ProxyUser", ui->proxyUserEdit->text());
    s->set("ProxyPass", ui->proxyPassEdit->text());

    APPLICATION->updateProxySettings(proxyType, ui->proxyAddrEdit->text(), ui->proxyPortEdit->value(), ui->proxyUserEdit->text(),
                                     ui->proxyPassEdit->text());
}
void ProxyPage::loadSettings()
{
    auto s = APPLICATION->settings();
    // Proxy
    QString proxyType = s->get("ProxyType").toString();
    if (proxyType == "Default")
        ui->proxyDefaultBtn->setChecked(true);
    else if (proxyType == "None")
        ui->proxyNoneBtn->setChecked(true);
    else if (proxyType == "SOCKS5")
        ui->proxySOCKS5Btn->setChecked(true);
    else if (proxyType == "HTTP")
        ui->proxyHTTPBtn->setChecked(true);

    ui->proxyAddrEdit->setText(s->get("ProxyAddr").toString());
    ui->proxyPortEdit->setValue(s->get("ProxyPort").value<uint16_t>());
    ui->proxyUserEdit->setText(s->get("ProxyUser").toString());
    ui->proxyPassEdit->setText(s->get("ProxyPass").toString());
}

void ProxyPage::retranslate()
{
    ui->retranslateUi(this);
}
