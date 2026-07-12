#pragma once

#include <QDialog>
#include "ui/pages/BasePageProvider.h"

class PageContainer;
class PageDialog : public QDialog {
    Q_OBJECT
   public:
    explicit PageDialog(BasePageProvider* pageProvider, QString defaultId = QString(), QWidget* parent = 0);
    virtual ~PageDialog() {}

   signals:
    void applied();

   private:
    void accept() override;
    void closeEvent(QCloseEvent* event) override;
    bool handleClose();

   private:
    PageContainer* m_container;
};
