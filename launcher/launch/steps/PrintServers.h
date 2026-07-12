#pragma once

#include <LoggedProcess.h>
#include <java/JavaChecker.h>
#include <launch/LaunchStep.h>
#include <QHostInfo>

class PrintServers : public LaunchStep {
    Q_OBJECT
   public:
    PrintServers(LaunchTask* parent, const QStringList& servers);

    virtual void executeTask();
    virtual bool canAbort() const;

   private:
    void resolveServer(const QHostInfo& host_info);
    QMap<QString, QString> m_server_to_address;
    QStringList m_servers;
};
