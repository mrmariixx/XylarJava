#include <fstream>
#include <string>

#include <launch/LaunchTask.h>
#include "PrintInstanceInfo.h"

#include "HardwareInfo.h"

#if defined(Q_OS_FREEBSD)
namespace {
void runSysctlHwModel(QStringList& log)
{
    char buff[512];
    FILE* hwmodel = popen("sysctl hw.model", "r");
    while (fgets(buff, 512, hwmodel) != NULL) {
        log << QString::fromUtf8(buff);
        break;
    }
    pclose(hwmodel);
}

void runPciconf(QStringList& log)
{
    char buff[512];
    std::string strcard;
    FILE* pciconf = popen("pciconf -lv -a vgapci0", "r");
    while (fgets(buff, 512, pciconf) != NULL) {
        if (strncmp(buff, "    vendor", 10) == 0) {
            std::string str(buff);
            strcard.append(str.substr(str.find_first_of("'") + 1, str.find_last_not_of("'") - (str.find_first_of("'") + 2)));
            strcard.append(" ");
        } else if (strncmp(buff, "    device", 10) == 0) {
            std::string str2(buff);
            strcard.append(str2.substr(str2.find_first_of("'") + 1, str2.find_last_not_of("'") - (str2.find_first_of("'") + 2)));
        }
        log << QString::fromStdString(strcard);
        break;
    }
    pclose(pciconf);
}
}  // namespace
#endif

void PrintInstanceInfo::executeTask()
{
    auto instance = m_parent->instance();
    QStringList log;

    log << "";
    log << "OS: " + QString("%1 | %2 | %3").arg(QSysInfo::prettyProductName(), QSysInfo::kernelType(), QSysInfo::kernelVersion());
#ifdef Q_OS_FREEBSD
    ::runSysctlHwModel(log);
    ::runPciconf(log);
#else
    log << "CPU: " + HardwareInfo::cpuInfo();
    log << QString("RAM: %1 MiB (available: %2 MiB)").arg(HardwareInfo::totalRamMiB()).arg(HardwareInfo::availableRamMiB());
#endif
    log.append(HardwareInfo::gpuInfo());
    log << "";

    logLines(log, MessageLevel::Launcher);
    logLines(instance->verboseDescription(m_session, m_targetToJoin), MessageLevel::Launcher);
    emitSucceeded();
}
