#include "Setting.h"
#include "settings/SettingsObject.h"

Setting::Setting(QStringList synonyms, QVariant defVal) : QObject(), m_synonyms(synonyms), m_defVal(defVal) {}

QVariant Setting::get() const
{
    SettingsObject* sbase = m_storage;
    if (!sbase) {
        return defValue();
    } else {
        QVariant test = sbase->retrieveValue(*this);
        if (!test.isValid())
            return defValue();
        return test;
    }
}

QVariant Setting::defValue() const
{
    return m_defVal;
}

void Setting::set(QVariant value)
{
    emit SettingChanged(*this, value);
}

void Setting::reset()
{
    emit settingReset(*this);
}
