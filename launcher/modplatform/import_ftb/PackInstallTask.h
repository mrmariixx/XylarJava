#pragma once

#include <QFuture>
#include <QFutureWatcher>

#include "InstanceTask.h"
#include "PackHelpers.h"

namespace FTBImportAPP {

class PackInstallTask : public InstanceTask {
    Q_OBJECT

   public:
    explicit PackInstallTask(const Modpack& pack) : m_pack(pack) {}
    virtual ~PackInstallTask() = default;

   protected:
    virtual void executeTask() override;

   private slots:
    void copySettings();

   private:
    QFuture<bool> m_copyFuture;
    QFutureWatcher<bool> m_copyFutureWatcher;

    const Modpack m_pack;
};

}  // namespace FTBImportAPP
