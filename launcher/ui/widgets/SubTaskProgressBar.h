#pragma once

#include <QWidget>
#include "QObjectPtr.h"

namespace Ui {
class SubTaskProgressBar;
}

class SubTaskProgressBar : public QWidget {
    Q_OBJECT

   public:
    static unique_qobject_ptr<SubTaskProgressBar> create(QWidget* parent = nullptr);

    SubTaskProgressBar(QWidget* parent = nullptr);
    ~SubTaskProgressBar();

    void setRange(int min, int max);
    void setValue(int value);
    void setStatus(QString status);
    void setDetails(QString details);

   private:
    Ui::SubTaskProgressBar* ui;
};
