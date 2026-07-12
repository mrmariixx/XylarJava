#pragma once

#include "net/Mode.h"
#include "tasks/Task.h"

class MinecraftInstance;

class MinecraftLoadAndCheck : public Task {
    Q_OBJECT
   public:
    explicit MinecraftLoadAndCheck(MinecraftInstance* inst, Net::Mode netmode);
    virtual ~MinecraftLoadAndCheck() = default;
    void executeTask() override;

    bool canAbort() const override;
   public slots:
    bool abort() override;

   private:
    MinecraftInstance* m_inst = nullptr;
    Task::Ptr m_task;
    Net::Mode m_netmode;
};
