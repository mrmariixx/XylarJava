#pragma once

#include "FusionTheme.h"

class BrightTheme : public FusionTheme {
   public:
    virtual ~BrightTheme() {}

    QString id() override;
    QString name() override;
    QString tooltip() override;
    bool hasStyleSheet() override;
    QString appStyleSheet() override;
    QPalette colorScheme() override;
    double fadeAmount() override;
    QColor fadeColor() override;
};
