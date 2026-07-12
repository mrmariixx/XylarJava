#pragma once

#include <QWizard>

namespace Ui {
class SetupWizard;
}

class BaseWizardPage;

class SetupWizard : public QWizard {
    Q_OBJECT

   public: /* con/destructors */
    explicit SetupWizard(QWidget* parent = 0);
    virtual ~SetupWizard();

    void changeEvent(QEvent* event) override;
    BaseWizardPage* getBasePage(int id);
    BaseWizardPage* getCurrentBasePage();

   private slots:
    void pageChanged(int id);

   private: /* methods */
    void retranslate();
};
