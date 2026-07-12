#include "SubTaskProgressBar.h"
#include "ui_SubTaskProgressBar.h"

unique_qobject_ptr<SubTaskProgressBar> SubTaskProgressBar::create(QWidget* parent)
{
    auto progress_bar = new SubTaskProgressBar(parent);
    return unique_qobject_ptr<SubTaskProgressBar>(progress_bar);
}

SubTaskProgressBar::SubTaskProgressBar(QWidget* parent) : QWidget(parent), ui(new Ui::SubTaskProgressBar)
{
    ui->setupUi(this);
}
SubTaskProgressBar::~SubTaskProgressBar()
{
    delete ui;
}

void SubTaskProgressBar::setRange(int min, int max)
{
    ui->progressBar->setRange(min, max);
}

void SubTaskProgressBar::setValue(int value)
{
    ui->progressBar->setValue(value);
}

void SubTaskProgressBar::setStatus(QString status)
{
    ui->statusLabel->setText(status);
}

void SubTaskProgressBar::setDetails(QString details)
{
    ui->statusDetailsLabel->setText(details);
}
