#include "GenericProfiler.h"

#include "BaseInstance.h"
#include "launch/LaunchTask.h"
#include "settings/SettingsObject.h"

class GenericProfiler : public BaseProfiler {
    Q_OBJECT
   public:
    GenericProfiler(SettingsObject* settings, BaseInstance* instance, QObject* parent = 0);

   protected:
    void beginProfilingImpl(LaunchTask* process);
};

GenericProfiler::GenericProfiler(SettingsObject* settings, BaseInstance* instance, QObject* parent)
    : BaseProfiler(settings, instance, parent)
{}

void GenericProfiler::beginProfilingImpl(LaunchTask* process)
{
    emit readyToLaunch(tr("Started process: %1").arg(process->pid()));
}

BaseExternalTool* GenericProfilerFactory::createTool(BaseInstance* instance, QObject* parent)
{
    return new GenericProfiler(globalSettings, instance, parent);
}
#include "GenericProfiler.moc"
