#pragma once

#include <QWidget>

#include "BaseInstance.h"
#include "ui/pages/BasePage.h"

namespace Ui {
class NotesPage;
}

class NotesPage : public QWidget, public BasePage {
    Q_OBJECT

   public:
    explicit NotesPage(BaseInstance* inst, QWidget* parent = 0);
    virtual ~NotesPage();
    virtual QString displayName() const override { return tr("Notes"); }
    virtual QIcon icon() const override
    {
        auto icon = QIcon::fromTheme("notes");
        if (icon.isNull())
            icon = QIcon::fromTheme("news");
        return icon;
    }
    virtual QString id() const override { return "notes"; }
    virtual bool apply() override;
    virtual QString helpPage() const override { return "Notes"; }
    void retranslate() override;

   private:
    Ui::NotesPage* ui;
    BaseInstance* m_inst;
};
