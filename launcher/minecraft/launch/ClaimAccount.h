#pragma once

#include <launch/LaunchStep.h>
#include <minecraft/auth/MinecraftAccount.h>

class ClaimAccount : public LaunchStep {
    Q_OBJECT
   public:
    explicit ClaimAccount(LaunchTask* parent, AuthSessionPtr session);
    virtual ~ClaimAccount() = default;

    void executeTask() override;
    void finalize() override;
    bool canAbort() const override { return false; }

   private:
    std::unique_ptr<UseLock> lock;
    MinecraftAccountPtr m_account;
};
