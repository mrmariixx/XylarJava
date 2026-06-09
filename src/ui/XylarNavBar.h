#pragma once

#include <QColor>
#include <QIcon>
#include <QPropertyAnimation>
#include <QString>
#include <QTimer>
#include <QVector>
#include <QWidget>

namespace xylar {

struct XylarNavItem
{
    QString id;
    QString label;
    QIcon icon;
};

class XylarNavBar final : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QRectF bubbleRect READ bubbleRect WRITE setBubbleRect)
    Q_PROPERTY(qreal pulse READ pulse WRITE setPulse)

public:
    explicit XylarNavBar(QWidget *parent = nullptr);

    void setItems(const QVector<XylarNavItem> &items);
    void setCurrentIndex(int index);
    [[nodiscard]] int currentIndex() const;

    [[nodiscard]] QRectF bubbleRect() const;
    void setBubbleRect(const QRectF &rect);

    [[nodiscard]] qreal pulse() const;
    void setPulse(qreal value);

signals:
    void currentChanged(const QString &id, int index);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    [[nodiscard]] QRectF capsuleRect() const;
    [[nodiscard]] QRectF itemRect(int index) const;
    [[nodiscard]] int itemAt(const QPointF &position) const;

    void animateBubbleTo(int index);
    void paintShell(QPainter &painter, const QRectF &rect);
    void paintBubble(QPainter &painter, const QRectF &rect);
    void paintItems(QPainter &painter);

    QVector<XylarNavItem> m_items;
    int m_currentIndex = 0;
    int m_hoverIndex = -1;
    QRectF m_bubbleRect;
    qreal m_pulse = 0.0;
    qreal m_phase = 0.0;
    QPointF m_mousePosition;
    QPropertyAnimation *m_bubbleAnimation = nullptr;
    QPropertyAnimation *m_pulseAnimation = nullptr;
    QTimer m_tick;
};

} // namespace xylar
