#include "EnsureAvailableMemory.h"

#include "HardwareInfo.h"
#include "ui/dialogs/CustomMessageBox.h"

EnsureAvailableMemory::EnsureAvailableMemory(LaunchTask* parent, MinecraftInstance* instance) : LaunchStep(parent), m_instance(instance) {}

void EnsureAvailableMemory::executeTask()
{
    const uint64_t available = HardwareInfo::availableRamMiB();
    const uint64_t min = m_instance->settings()->get("MinMemAlloc").toUInt();
    const uint64_t max = m_instance->settings()->get("MaxMemAlloc").toUInt();
    const uint64_t required = std::max(min, max);

    if (static_cast<double>(required) * 0.9 > static_cast<double>(available)) {
        bool shouldAbort = false;

        if (m_instance->settings()->get("LowMemWarning").toBool()) {
            auto* dialog = CustomMessageBox::selectable(
                nullptr, tr("Not enough RAM"),
                tr("There is not enough RAM available to launch this instance with the current memory settings.\n\n"
                   "Required: %1 MiB\nAvailable: %2 MiB\n\n"
                   "Continue anyway? This may cause slowdowns in the game and your system.")
                    .arg(required)
                    .arg(available),
                QMessageBox::Icon::Warning, QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No,
                QMessageBox::StandardButton::No);

            shouldAbort = dialog->exec() == QMessageBox::No;
            dialog->deleteLater();
        }

        const auto message = tr("Not enough RAM available to launch this instance");
        if (shouldAbort) {
            emit logLine(message, MessageLevel::Fatal);
            emitFailed(message);
            return;
        }

        emit logLine(message, MessageLevel::Warning);
    }

    emitSucceeded();
}
