#pragma once

#include "BaseProfiler.h"

class GenericProfilerFactory : public BaseProfilerFactory {
   public:
    QString name() const override { return "Generic"; }
    void registerSettings([[maybe_unused]] SettingsObject* settings) override {};
    BaseExternalTool* createTool(BaseInstance* instance, QObject* parent = 0) override;
    bool check([[maybe_unused]] QString* error) override { return true; };
    bool check([[maybe_unused]] const QString& path, [[maybe_unused]] QString* error) override { return true; };
};
