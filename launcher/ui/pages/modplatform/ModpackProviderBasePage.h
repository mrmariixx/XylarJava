#pragma once

#include "ui/pages/BasePage.h"

class ModpackProviderBasePage : public BasePage {
   public:
    /** Programatically set the term in the search bar. */
    virtual void setSearchTerm(QString) = 0;
    /** Get the current term in the search bar. */
    virtual QString getSerachTerm() const = 0;
};
