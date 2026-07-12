#include "PassthroughSetting.h"

PassthroughSetting::PassthroughSetting(std::shared_ptr<Setting> other, std::shared_ptr<Setting> gate)
    : Setting(other->configKeys(), QVariant())
{
    Q_ASSERT(other);
    m_other = other;
    m_gate = gate;
}

bool PassthroughSetting::isOverriding() const
{
    if (!m_gate) {
        return false;
    }
    return m_gate->get().toBool();
}

QVariant PassthroughSetting::defValue() const
{
    if (isOverriding()) {
        return m_other->get();
    }
    return m_other->defValue();
}

QVariant PassthroughSetting::get() const
{
    if (isOverriding()) {
        return Setting::get();
    }
    return m_other->get();
}

void PassthroughSetting::reset()
{
    if (isOverriding()) {
        Setting::reset();
    }
    m_other->reset();
}

void PassthroughSetting::set(QVariant value)
{
    if (isOverriding()) {
        Setting::set(value);
    }
    m_other->set(value);
}
