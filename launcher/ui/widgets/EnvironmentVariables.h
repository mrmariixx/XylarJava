#pragma once

#include <QMap>
#include <QWidget>

namespace Ui {
class EnvironmentVariables;
}

class EnvironmentVariables : public QWidget {
    Q_OBJECT

   public:
    explicit EnvironmentVariables(QWidget* state = nullptr);
    ~EnvironmentVariables() override;
    void initialize(bool instance, bool override, const QMap<QString, QVariant>& value);
    bool eventFilter(QObject* watched, QEvent* event) override;

    void retranslate();
    bool override() const;
    QMap<QString, QVariant> value() const;

   private:
    Ui::EnvironmentVariables* ui;
};
