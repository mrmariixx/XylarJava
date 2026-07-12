#pragma once

#include <QDialog>
#include <QModelIndex>
#include <memory>
#include "FastFileIconProvider.h"
#include "FileIgnoreProxy.h"

class BaseInstance;

namespace Ui {
class ExportInstanceDialog;
}

class ExportInstanceDialog : public QDialog {
    Q_OBJECT

   public:
    explicit ExportInstanceDialog(BaseInstance* instance, QWidget* parent = 0);
    ~ExportInstanceDialog();

    virtual void done(int result);

   private:
    void doExport();
    QString ignoreFileName();

   private:
    Ui::ExportInstanceDialog* m_ui;
    BaseInstance* m_instance;
    FileIgnoreProxy* m_proxyModel;
    FastFileIconProvider m_icons;

   private slots:
    void rowsInserted(QModelIndex parent, int top, int bottom);
};
