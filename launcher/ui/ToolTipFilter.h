#pragma once

#include <QEvent>
#include <QObject>

class ToolTipFilter : public QObject {
    Q_OBJECT
   protected:
    bool eventFilter(QObject* obj, QEvent* event);
};
