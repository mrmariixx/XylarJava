#pragma once
#include <MessageLevel.h>
#include <QMap>
#include <QPalette>
#include <QString>

class QStyle;

struct LogColors {
    QMap<MessageLevel, QColor> background;
    QMap<MessageLevel, QColor> foreground;
};

// TODO: rename to Theme; this is not an interface as it contains method implementations
// TODO: make methods const
class ITheme {
   public:
    virtual ~ITheme() {}
    virtual void apply(bool initial);
    virtual QString id() = 0;
    virtual QString name() = 0;
    virtual QString tooltip() = 0;
    virtual bool hasStyleSheet() = 0;
    virtual QString appStyleSheet() = 0;
    virtual QString qtTheme() = 0;
    virtual QPalette colorScheme() = 0;
    virtual QColor fadeColor() = 0;
    virtual double fadeAmount() = 0;
    virtual LogColors logColorScheme() { return defaultLogColors(colorScheme()); }
    virtual QStringList searchPaths() { return {}; }

    static QPalette fadeInactive(QPalette in, qreal bias, QColor color);
    static LogColors defaultLogColors(const QPalette& palette);
};
