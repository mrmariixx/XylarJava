#pragma once

#include <functional>
#include "ui/pages/BasePage.h"

class BasePageProvider {
   public:
    virtual QList<BasePage*> getPages() = 0;
    virtual QString dialogTitle() = 0;
};

class GenericPageProvider : public BasePageProvider {
    using PageCreator = std::function<BasePage*()>;

   public:
    explicit GenericPageProvider(const QString& dialogTitle) : m_dialogTitle(dialogTitle) {}
    virtual ~GenericPageProvider() {}

    QList<BasePage*> getPages() override
    {
        QList<BasePage*> pages;
        for (PageCreator creator : m_creators) {
            pages.append(creator());
        }
        return pages;
    }
    QString dialogTitle() override { return m_dialogTitle; }

    void setDialogTitle(const QString& title) { m_dialogTitle = title; }
    void addPageCreator(PageCreator page) { m_creators.append(page); }

    template <typename PageClass>
    void addPage()
    {
        addPageCreator([]() { return new PageClass(); });
    }

   private:
    QList<PageCreator> m_creators;
    QString m_dialogTitle;
};
