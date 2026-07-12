#pragma once

#include <QDialog>
#include <QHash>
#include <QUuid>
#include <memory>

#include "QObjectPtr.h"
#include "tasks/Task.h"

#include "ui/widgets/SubTaskProgressBar.h"

class Task;
class SequentialTask;

namespace Ui {
class ProgressDialog;
}

class ProgressDialog : public QDialog {
    Q_OBJECT

   public:
    explicit ProgressDialog(QWidget* parent = 0);
    ~ProgressDialog();

    void updateSize(bool recenterParent = false);

    int execWithTask(Task* task);
    int execWithTask(std::unique_ptr<Task>&& task);
    int execWithTask(std::unique_ptr<Task>& task);

    void setSkipButton(bool present, QString label = QString());

    Task* getTask();

   public slots:
    void onTaskStarted();
    void onTaskFailed(QString failure);
    void onTaskSucceeded();

    void changeStatus(const QString& status);
    void changeProgress(qint64 current, qint64 total);
    void changeStepProgress(TaskStepProgress const& task_progress);

   private slots:
    void on_skipButton_clicked(bool checked);

   protected:
    virtual void keyPressEvent(QKeyEvent* e);
    virtual void closeEvent(QCloseEvent* e);

   private:
    bool handleImmediateResult(QDialog::DialogCode& result);
    void addTaskProgress(TaskStepProgress const& progress);

   private:
    Ui::ProgressDialog* ui;

    Task* m_task;

    QList<QMetaObject::Connection> m_taskConnections;

    bool m_is_multi_step = false;
    QHash<QUuid, SubTaskProgressBar*> taskProgress;
};
