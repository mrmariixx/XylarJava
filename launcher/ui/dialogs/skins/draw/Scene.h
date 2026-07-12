#pragma once

#include "ui/dialogs/skins/draw/BoxGeometry.h"

#include <QOpenGLTexture>
namespace opengl {
class Scene : protected QOpenGLFunctions {
   public:
    Scene(const QImage& skin, bool slim, const QImage& cape);
    virtual ~Scene();

    void draw(QOpenGLShaderProgram* program);
    void setSkin(const QImage& skin);
    void setCape(const QImage& cape);
    void setMode(bool slim);
    void setCapeVisible(bool visible);
    void setElytraVisible(bool elytraVisible);

   private:
    QList<BoxGeometry*> m_staticComponents;
    QList<BoxGeometry*> m_normalArms;
    QList<BoxGeometry*> m_slimArms;
    QList<BoxGeometry*> m_staticComponentsOverlay;
    QList<BoxGeometry*> m_normalArmsOverlay;
    QList<BoxGeometry*> m_slimArmsOverlay;
    BoxGeometry* m_cape = nullptr;
    QList<BoxGeometry*> m_elytra;
    QOpenGLTexture* m_skinTexture = nullptr;
    QOpenGLTexture* m_capeTexture = nullptr;
    bool m_slim = false;
    bool m_capeVisible = false;
    bool m_elytraVisible = false;
};
}  // namespace opengl
