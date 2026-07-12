#include "CustomCommands.h"
#include "ui_CustomCommands.h"

CustomCommands::~CustomCommands()
{
    delete ui;
}

CustomCommands::CustomCommands(QWidget* parent) : QWidget(parent), ui(new Ui::CustomCommands)
{
    ui->setupUi(this);
    connect(ui->overrideCheckBox, &QCheckBox::toggled, ui->customCommandsWidget, &QWidget::setEnabled);
}

void CustomCommands::initialize(bool checkable, bool checked, const QString& prelaunch, const QString& wrapper, const QString& postexit)
{
    ui->overrideCheckBox->setVisible(checkable);
    if (checkable) {
        ui->overrideCheckBox->setChecked(checked);
    }
    ui->preLaunchCmdTextBox->setText(prelaunch);
    ui->wrapperCmdTextBox->setText(wrapper);
    ui->postExitCmdTextBox->setText(postexit);
}

void CustomCommands::retranslate()
{
    ui->retranslateUi(this);
}

bool CustomCommands::checked() const
{
    return ui->overrideCheckBox->isChecked();
}

QString CustomCommands::prelaunchCommand() const
{
    return ui->preLaunchCmdTextBox->text();
}

QString CustomCommands::wrapperCommand() const
{
    return ui->wrapperCmdTextBox->text();
}

QString CustomCommands::postexitCommand() const
{
    return ui->postExitCmdTextBox->text();
}
