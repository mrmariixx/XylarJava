#pragma once

#include "FusionTheme.h"

class Win7AeroTheme : public FusionTheme {
   public:
    virtual ~Win7AeroTheme() {}

    QString id() override;
    QString name() override;
    QString tooltip() override;
    bool hasStyleSheet() override;
    QString appStyleSheet() override;
    QPalette colorScheme() override;
    QString qtTheme() override;
    double fadeAmount() override;
    QColor fadeColor() override;
};
