#pragma once

#include <QObjectPtr.h>
#include <launch/LaunchStep.h>
#include <QDnsLookup>

#include "minecraft/launch/MinecraftTarget.h"

class LookupServerAddress : public LaunchStep {
    Q_OBJECT
   public:
    explicit LookupServerAddress(LaunchTask* parent);
    virtual ~LookupServerAddress() = default;

    virtual void executeTask();
    virtual bool abort();
    virtual bool canAbort() const { return true; }

    void setLookupAddress(const QString& lookupAddress);
    void setOutputAddressPtr(MinecraftTarget::Ptr output);

   private slots:
    void on_dnsLookupFinished();

   private:
    void resolve(const QString& address, quint16 port);

    QDnsLookup* m_dnsLookup;
    QString m_lookupAddress;
    MinecraftTarget::Ptr m_output;
};
