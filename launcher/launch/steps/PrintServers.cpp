#include "PrintServers.h"
#include "QHostInfo"

PrintServers::PrintServers(LaunchTask* parent, const QStringList& servers) : LaunchStep(parent)
{
    m_servers = servers;
}

void PrintServers::executeTask()
{
    for (QString server : m_servers) {
        QHostInfo::lookupHost(server, this, &PrintServers::resolveServer);
    }
}

void PrintServers::resolveServer(const QHostInfo& host_info)
{
    QString server = host_info.hostName();
    QString addresses = server + " resolves to:\n  ";

    if (!host_info.addresses().isEmpty()) {
        for (QHostAddress address : host_info.addresses()) {
            addresses += address.toString();
            if (!host_info.addresses().endsWith(address)) {
                addresses += ", ";
            }
        }
    } else {
        addresses += "N/A";
    }
    addresses += "\n";

    m_server_to_address.insert(server, addresses);

    // print server info in order once all servers are resolved
    if (m_server_to_address.size() >= m_servers.size()) {
        for (QString serv : m_servers) {
            emit logLine(m_server_to_address.value(serv), MessageLevel::Launcher);
        }
        emitSucceeded();
    }
}

bool PrintServers::canAbort() const
{
    return true;
}
