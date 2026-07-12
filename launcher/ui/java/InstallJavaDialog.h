#pragma once

#include <QDialog>
#include "BaseInstance.h"
#include "ui/pages/BasePageProvider.h"

class MinecraftInstance;
class PageContainer;
class PackProfile;
class QDialogButtonBox;

namespace Java {
class InstallDialog final : public QDialog, private BasePageProvider {
    Q_OBJECT

   public:
    explicit InstallDialog(const QString& uid = QString(), BaseInstance* instance = nullptr, QWidget* parent = nullptr);

    QList<BasePage*> getPages() override;
    QString dialogTitle() override;

    void validate(BasePage* selected);
    void done(int result) override;

   private:
    PageContainer* container;
    QDialogButtonBox* buttons;
};
}  // namespace Java
