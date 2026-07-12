#pragma once

#include <QIcon>
#include <QString>
#include <functional>
#include <memory>

#include "BasePageContainer.h"

class BasePage {
   public:
    using updateExtraInfoFunc = std::function<void(QString, QString)>;
    virtual ~BasePage() {}
    virtual QString id() const = 0;
    virtual QString displayName() const = 0;
    virtual QIcon icon() const = 0;
    virtual bool apply() { return true; }
    virtual bool shouldDisplay() const { return true; }
    virtual QString helpPage() const { return QString(); }
    void opened()
    {
        isOpened = true;
        openedImpl();
    }
    void closed()
    {
        isOpened = false;
        closedImpl();
    }
    virtual void openedImpl() {}
    virtual void closedImpl() {}
    virtual void setParentContainer(BasePageContainer* container) { m_container = container; };
    virtual void retranslate() {}

   public:
    int stackIndex = -1;
    int listIndex = -1;
    updateExtraInfoFunc updateExtraInfo;

   protected:
    BasePageContainer* m_container = nullptr;
    bool isOpened = false;
};

using BasePagePtr = std::shared_ptr<BasePage>;
