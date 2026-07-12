#pragma once

#include <QFileIconProvider>

class FastFileIconProvider : public QFileIconProvider {
   public:
    QIcon icon(const QFileInfo& info) const override;
};
