#pragma once

#include <QString>
#include <QWidget>

#include "ui/pages/BasePage.h"

namespace Ui {
class GameOptionsPage;
}

class GameOptions;
class MinecraftInstance;

class GameOptionsPage : public QWidget, public BasePage {
    Q_OBJECT

   public:
    explicit GameOptionsPage(MinecraftInstance* inst, QWidget* parent = 0);
    virtual ~GameOptionsPage();

    void openedImpl() override;
    void closedImpl() override;

    virtual QString displayName() const override { return tr("Game Options"); }
    virtual QIcon icon() const override { return QIcon::fromTheme("settings"); }
    virtual QString id() const override { return "gameoptions"; }
    virtual QString helpPage() const override { return "Game-Options-management"; }
    void retranslate() override;

   private:  // data
    Ui::GameOptionsPage* ui = nullptr;
    std::shared_ptr<GameOptions> m_model;
};
