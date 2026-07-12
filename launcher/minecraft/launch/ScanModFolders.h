#pragma once

#include <launch/LaunchStep.h>
#include <memory>

class ScanModFolders : public LaunchStep {
    Q_OBJECT
   public:
    explicit ScanModFolders(LaunchTask* parent) : LaunchStep(parent) {};
    virtual ~ScanModFolders() {};

    virtual void executeTask() override;
    virtual bool canAbort() const override { return false; }
   private slots:
    void coreModsDone();
    void modsDone();
    void nilModsDone();

   private:
    void checkDone();

   private:  // DATA
    bool m_modsDone = false;
    bool m_nilModsDone = false;
    bool m_coreModsDone = false;
};
