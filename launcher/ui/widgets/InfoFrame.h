#pragma once

#include <QFrame>

#include "minecraft/mod/DataPack.h"
#include "minecraft/mod/Mod.h"
#include "minecraft/mod/ResourcePack.h"
#include "minecraft/mod/TexturePack.h"

namespace Ui {
class InfoFrame;
}

class InfoFrame : public QFrame {
    Q_OBJECT

   public:
    InfoFrame(QWidget* parent = nullptr);
    ~InfoFrame() override;

    void setName(QString text = {});
    void setDescription(QString text = {});
    void setImage(QPixmap img = {});
    void setLicense(QString text = {});
    void setIssueTracker(QString text = {});

    void clear();

    void updateWithMod(Mod const& m);
    void updateWithResource(Resource const& resource);
    void updateWithResourcePack(ResourcePack& rp);
    void updateWithDataPack(DataPack& rp);
    void updateWithTexturePack(TexturePack& tp);

    static QString renderColorCodes(QString input);

   public slots:
    void descriptionEllipsisHandler(QString link);
    void licenseEllipsisHandler(QString link);
    void boxClosed(int result);

   private:
    void updateHiddenState();
    void resetScroll();

   private:
    Ui::InfoFrame* ui;
    QString m_description;
    QString m_license;
    class QMessageBox* m_current_box = nullptr;
};
