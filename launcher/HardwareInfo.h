#pragma once

#include <QString>
#include <cstdint>

namespace HardwareInfo {
QString cpuInfo();
uint64_t totalRamMiB();
uint64_t availableRamMiB();
QStringList gpuInfo();
}  // namespace HardwareInfo
