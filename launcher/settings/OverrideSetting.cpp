#include "OverrideSetting.h"

OverrideSetting::OverrideSetting(std::shared_ptr<Setting> other, std::shared_ptr<Setting> gate) : Setting(other->configKeys(), QVariant())
{
    Q_ASSERT(other);
    Q_ASSERT(gate);
    m_other = other;
    m_gate = gate;
}

bool OverrideSetting::isOverriding() const
{
    return m_gate->get().toBool();
}

QVariant OverrideSetting::defValue() const
{
    return m_other->get();
}

QVariant OverrideSetting::get() const
{
    if (isOverriding()) {
        return Setting::get();
    }
    return m_other->get();
}

void OverrideSetting::reset()
{
    Setting::reset();
}

void OverrideSetting::set(QVariant value)
{
    Setting::set(value);
}
