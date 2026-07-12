#pragma once

#include <QDialog>
#include <QList>
#include "minecraft/mod/Mod.h"
#include "modplatform/helpers/ExportToModList.h"

namespace Ui {
class ExportToModListDialog;
}

class ExportToModListDialog : public QDialog {
    Q_OBJECT

   public:
    explicit ExportToModListDialog(QString name, QList<Mod*> mods, QWidget* parent = nullptr);
    ~ExportToModListDialog();

    void done(int result) override;

   protected slots:
    void formatChanged(int index);
    void triggerImp();
    void trigger(int) { triggerImp(); };
    void addExtra(ExportToModList::OptionalData option);

   private:
    QString extension();
    void enableCustom(bool enabled);

    QList<Mod*> m_mods;
    bool m_template_changed;
    QString m_name;
    ExportToModList::Formats m_format = ExportToModList::Formats::HTML;
    Ui::ExportToModListDialog* ui;
    static const QHash<ExportToModList::Formats, QString> exampleLines;
};
