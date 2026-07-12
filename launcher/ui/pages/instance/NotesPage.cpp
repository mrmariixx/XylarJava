#include "NotesPage.h"
#include <QTabBar>
#include "ui_NotesPage.h"

NotesPage::NotesPage(BaseInstance* inst, QWidget* parent) : QWidget(parent), ui(new Ui::NotesPage), m_inst(inst)
{
    ui->setupUi(this);
    ui->noteEditor->setText(m_inst->notes());
}

NotesPage::~NotesPage()
{
    delete ui;
}

bool NotesPage::apply()
{
    m_inst->setNotes(ui->noteEditor->toPlainText());
    return true;
}

void NotesPage::retranslate()
{
    ui->retranslateUi(this);
}
