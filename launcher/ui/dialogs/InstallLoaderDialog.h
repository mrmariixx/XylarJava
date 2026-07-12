#pragma once

#include <QDialog>
#include "ui/pages/BasePageProvider.h"

class MinecraftInstance;
class PageContainer;
class PackProfile;
class QDialogButtonBox;

class InstallLoaderDialog final : public QDialog, protected BasePageProvider {
    Q_OBJECT

   public:
    explicit InstallLoaderDialog(PackProfile* instance, const QString& uid = QString(), QWidget* parent = nullptr);

    QList<BasePage*> getPages() override;
    QString dialogTitle() override;

    void validate(BasePage* page);
    void done(int result) override;

   private:
    PackProfile* profile;
    PageContainer* container;
    QDialogButtonBox* buttons;
};
