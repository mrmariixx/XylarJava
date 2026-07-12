#pragma once

#include <launch/LaunchStep.h>
#include <launch/LaunchTask.h>
#include "meta/Version.h"
#include "minecraft/MinecraftInstance.h"
#include "tasks/Task.h"

class AutoInstallJava : public LaunchStep {
    Q_OBJECT

   public:
    explicit AutoInstallJava(LaunchTask* parent);
    ~AutoInstallJava() override = default;

    void executeTask() override;
    bool canAbort() const override { return m_current_task ? m_current_task->canAbort() : false; }
    bool abort() override;

   protected:
    void setJavaPath(QString path);
    void setJavaPathFromPartial();
    void downloadJava(Meta::Version::Ptr version, QString javaName);
    void tryNextMajorJava();

   private:
    MinecraftInstance* m_instance;
    Task::Ptr m_current_task;

    qsizetype m_majorJavaVersionIndex = 0;
    const QString m_supported_arch;
};
