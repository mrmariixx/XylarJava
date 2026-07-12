#pragma once

#include <QProxyStyle>
#include <iostream>

/// Used to override platform-specific behaviours which the launcher does work well with.
class HintOverrideProxyStyle : public QProxyStyle {
    Q_OBJECT
   public:
    explicit HintOverrideProxyStyle(QStyle* style);

    int styleHint(QStyle::StyleHint hint,
                  const QStyleOption* option = nullptr,
                  const QWidget* widget = nullptr,
                  QStyleHintReturn* returnData = nullptr) const override;
};
