#include "ui/themes/CatPainter.h"
#include <QPixmap>
#include "Application.h"
#include "settings/SettingsObject.h"

CatPainter::CatPainter(const QString& path, QObject* parent) : QObject(parent)
{
    // Attempt to load as a movie
    m_movie = new QMovie(path, QByteArray(), this);
    if (m_movie->isValid()) {
        // Start the animation if it's a valid movie file
        connect(m_movie, &QMovie::frameChanged, this, &CatPainter::updateFrame);
        m_movie->start();
    } else {
        // Otherwise, load it as a static image
        delete m_movie;
        m_movie = nullptr;

        m_image = QPixmap(path);
    }
}

void CatPainter::paint(QPainter* painter, const QRect& viewport)
{
    QPixmap frame = m_image;
    if (m_movie && m_movie->isValid()) {
        frame = m_movie->currentPixmap();
    }

    auto fit = APPLICATION->settings()->get("CatFit").toString();
    painter->setOpacity(APPLICATION->settings()->get("CatOpacity").toFloat() / 100);
    int widWidth = viewport.width();
    int widHeight = viewport.height();
    auto aspectMode = Qt::IgnoreAspectRatio;
    if (fit == "fill") {
        aspectMode = Qt::KeepAspectRatio;
    } else if (fit == "fit") {
        aspectMode = Qt::KeepAspectRatio;
        if (frame.width() < widWidth)
            widWidth = frame.width();
        if (frame.height() < widHeight)
            widHeight = frame.height();
    }
    auto pixmap = frame.scaled(widWidth, widHeight, aspectMode, Qt::SmoothTransformation);
    QRect rectOfPixmap = pixmap.rect();
    rectOfPixmap.moveBottomRight(viewport.bottomRight());
    painter->drawPixmap(rectOfPixmap.topLeft(), pixmap);
    painter->setOpacity(1.0);
};
