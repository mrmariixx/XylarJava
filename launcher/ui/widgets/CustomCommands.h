#pragma once

#include <QWidget>

namespace Ui {
class CustomCommands;
}

class CustomCommands : public QWidget {
    Q_OBJECT

   public:
    explicit CustomCommands(QWidget* parent = 0);
    virtual ~CustomCommands();
    void initialize(bool checkable, bool checked, const QString& prelaunch, const QString& wrapper, const QString& postexit);

    void retranslate();
    bool checked() const;
    QString prelaunchCommand() const;
    QString wrapperCommand() const;
    QString postexitCommand() const;

   private:
    Ui::CustomCommands* ui;
};
