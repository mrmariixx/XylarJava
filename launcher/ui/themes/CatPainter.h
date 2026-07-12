#pragma once

#include <QMovie>
#include <QPainter>
#include <QPixmap>
#include <QString>

class CatPainter : public QObject {
    Q_OBJECT
   public:
    CatPainter(const QString& path, QObject* parent = nullptr);
    virtual ~CatPainter() = default;
    void paint(QPainter*, const QRect&);

   signals:
    void updateFrame();

   private:
    QMovie* m_movie = nullptr;
    QPixmap m_image;
};
