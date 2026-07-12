#pragma once

#include "ITheme.h"

class SystemTheme : public ITheme {
   public:
    SystemTheme(const QString& styleName, const QPalette& defaultPalette, bool isDefaultTheme);
    virtual ~SystemTheme() {}
    void apply(bool initial) override;

    QString id() override;
    QString name() override;
    QString tooltip() override;
    QString qtTheme() override;
    bool hasStyleSheet() override;
    QString appStyleSheet() override;
    QPalette colorScheme() override;
    double fadeAmount() override;
    QColor fadeColor() override;

   private:
    QPalette m_colorPalette;
    QString m_widgetTheme;
    QString m_themeName;
};
