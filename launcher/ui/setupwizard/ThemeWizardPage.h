#pragma once

#include <ui/widgets/AppearanceWidget.h>
#include <QHBoxLayout>
#include <QWidget>
#include "BaseWizardPage.h"

class ThemeWizardPage : public BaseWizardPage {
    Q_OBJECT

   public:
    ThemeWizardPage(QWidget* parent = nullptr) : BaseWizardPage(parent)
    {
        auto layout = new QVBoxLayout(this);
        layout->addWidget(&widget);
        layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
        layout->setContentsMargins(0, 0, 0, 0);
        setLayout(layout);

        setTitle(tr("Appearance"));
        setSubTitle(tr("Select theme and icons to use"));
    }

    bool validatePage() override { return true; };
    void retranslate() override { widget.retranslateUi(); }

   private:
    AppearanceWidget widget{ true };
};
