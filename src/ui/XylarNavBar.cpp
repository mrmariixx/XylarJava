#include "ui/XylarNavBar.h"

#include <QEvent>
#include <QEasingCurve>
#include <QLinearGradient>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QRadialGradient>
#include <QtGlobal>
#include <QtMath>

namespace xylar {
namespace {

QColor alpha(QColor color, int value)
{
    color.setAlpha(value);
    return color;
}

} // namespace

XylarNavBar::XylarNavBar(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedHeight(96);
    setMinimumWidth(430);

    m_bubbleAnimation = new QPropertyAnimation(this, "bubbleRect", this);
    m_bubbleAnimation->setDuration(470);
    m_bubbleAnimation->setEasingCurve(QEasingCurve::OutBack);

    m_pulseAnimation = new QPropertyAnimation(this, "pulse", this);
    m_pulseAnimation->setDuration(1400);
    m_pulseAnimation->setStartValue(0.0);
    m_pulseAnimation->setEndValue(1.0);
    m_pulseAnimation->setEasingCurve(QEasingCurve::InOutSine);
    m_pulseAnimation->setLoopCount(-1);
    m_pulseAnimation->start();

    connect(&m_tick, &QTimer::timeout, this, [this]() {
        m_phase += 0.045;
        if (m_phase > 6.28318) {
            m_phase = 0.0;
        }
        update();
    });
    m_tick.start(16);
}

void XylarNavBar::setItems(const QVector<XylarNavItem> &items)
{
    m_items = items;
    m_currentIndex = m_items.isEmpty() ? -1 : 0;
    m_hoverIndex = -1;
    m_bubbleRect = m_currentIndex >= 0 ? itemRect(m_currentIndex).adjusted(2, 2, -2, -2) : QRectF();
    updateGeometry();
    update();
}

void XylarNavBar::setCurrentIndex(int index)
{
    if (index < 0 || index >= m_items.size() || index == m_currentIndex) {
        return;
    }

    m_currentIndex = index;
    animateBubbleTo(index);
    emit currentChanged(m_items.at(index).id, index);
}

int XylarNavBar::currentIndex() const
{
    return m_currentIndex;
}

QRectF XylarNavBar::bubbleRect() const
{
    return m_bubbleRect;
}

void XylarNavBar::setBubbleRect(const QRectF &rect)
{
    m_bubbleRect = rect;
    update();
}

qreal XylarNavBar::pulse() const
{
    return m_pulse;
}

void XylarNavBar::setPulse(qreal value)
{
    m_pulse = value;
    update();
}

void XylarNavBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    paintShell(painter, capsuleRect());
    if (!m_bubbleRect.isNull()) {
        paintBubble(painter, m_bubbleRect);
    }
    paintItems(painter);
}

void XylarNavBar::mouseMoveEvent(QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_mousePosition = event->position();
#else
    m_mousePosition = event->localPos();
#endif
    const int index = itemAt(m_mousePosition);
    if (index != m_hoverIndex) {
        m_hoverIndex = index;
        animateBubbleTo(m_hoverIndex >= 0 ? m_hoverIndex : m_currentIndex);
    }
    update();
}

void XylarNavBar::mousePressEvent(QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const int index = itemAt(event->position());
#else
    const int index = itemAt(event->localPos());
#endif
    if (index >= 0) {
        setCurrentIndex(index);
    }
}

void XylarNavBar::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);
    m_hoverIndex = -1;
    animateBubbleTo(m_currentIndex);
}

void XylarNavBar::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (m_currentIndex >= 0) {
        m_bubbleRect = itemRect(m_currentIndex).adjusted(2, 2, -2, -2);
    }
}

QRectF XylarNavBar::capsuleRect() const
{
    return QRectF(14, 12, width() - 28, height() - 24);
}

QRectF XylarNavBar::itemRect(int index) const
{
    if (m_items.isEmpty() || index < 0 || index >= m_items.size()) {
        return QRectF();
    }

    const QRectF inner = capsuleRect().adjusted(10, 8, -10, -8);
    const qreal itemWidth = inner.width() / static_cast<qreal>(m_items.size());
    return QRectF(inner.left() + itemWidth * index, inner.top(), itemWidth, inner.height());
}

int XylarNavBar::itemAt(const QPointF &position) const
{
    for (int i = 0; i < m_items.size(); ++i) {
        if (itemRect(i).contains(position)) {
            return i;
        }
    }
    return -1;
}

void XylarNavBar::animateBubbleTo(int index)
{
    if (index < 0 || index >= m_items.size()) {
        return;
    }

    const QRectF target = itemRect(index).adjusted(2, 2, -2, -2);
    if (m_bubbleRect.isNull()) {
        m_bubbleRect = target;
    }

    m_bubbleAnimation->stop();
    m_bubbleAnimation->setStartValue(m_bubbleRect);
    m_bubbleAnimation->setEndValue(target);
    m_bubbleAnimation->start();
}

void XylarNavBar::paintShell(QPainter &painter, const QRectF &rect)
{
    const qreal radius = rect.height() / 2.0;

    QPainterPath shadow;
    shadow.addRoundedRect(rect.adjusted(1, 8, -1, 12), radius, radius);
    painter.fillPath(shadow, QColor(0, 0, 0, 78));

    QPainterPath shell;
    shell.addRoundedRect(rect, radius, radius);

    QLinearGradient base(rect.topLeft(), rect.bottomRight());
    base.setColorAt(0.0, QColor(255, 255, 255, 88));
    base.setColorAt(0.35, QColor(163, 255, 224, 44));
    base.setColorAt(0.7, QColor(96, 151, 137, 46));
    base.setColorAt(1.0, QColor(255, 255, 255, 28));
    painter.fillPath(shell, base);

    QLinearGradient rim(rect.left(), rect.top(), rect.left(), rect.bottom());
    rim.setColorAt(0.0, QColor(255, 255, 255, 126));
    rim.setColorAt(0.45, QColor(255, 255, 255, 20));
    rim.setColorAt(1.0, QColor(0, 0, 0, 30));
    painter.fillPath(shell, rim);

    painter.setPen(QPen(QColor(255, 255, 255, 130), 1.15));
    painter.drawPath(shell);
    painter.setPen(QPen(QColor(123, 255, 226, 44), 1.0));
    painter.drawRoundedRect(rect.adjusted(5, 5, -5, -5), radius - 5, radius - 5);
}

void XylarNavBar::paintBubble(QPainter &painter, const QRectF &rect)
{
    const qreal wave = (qSin(m_phase) + 1.0) * 0.5;
    const qreal stretch = m_hoverIndex >= 0 ? 7.0 + wave * 4.0 : 4.0;
    const QRectF bubble = rect.adjusted(-stretch, -4 - wave * 2.0, stretch, 5 + wave * 2.0);
    const qreal radius = bubble.height() / 2.0;
    const QPointF focus = m_hoverIndex >= 0 ? m_mousePosition : bubble.center();

    QPainterPath aura;
    aura.addRoundedRect(bubble.adjusted(-10, -8, 10, 10), radius + 12, radius + 12);
    painter.fillPath(aura, QColor(135, 255, 220, 22 + static_cast<int>(m_pulse * 18)));

    QPainterPath bubblePath;
    bubblePath.addRoundedRect(bubble, radius, radius);

    QRadialGradient fill(focus, bubble.width() * 0.72, focus);
    fill.setColorAt(0.0, QColor(255, 255, 246, 186));
    fill.setColorAt(0.32, QColor(184, 255, 232, 125));
    fill.setColorAt(0.68, QColor(99, 186, 169, 82));
    fill.setColorAt(1.0, QColor(255, 255, 255, 34));
    painter.fillPath(bubblePath, fill);

    QLinearGradient shade(bubble.topLeft(), bubble.bottomLeft());
    shade.setColorAt(0.0, QColor(255, 255, 255, 145));
    shade.setColorAt(0.52, QColor(255, 255, 255, 22));
    shade.setColorAt(1.0, QColor(0, 0, 0, 32));
    painter.fillPath(bubblePath, shade);

    const QRectF highlight = bubble.adjusted(18, 8, -18, -bubble.height() * 0.6);
    QPainterPath shine;
    shine.addRoundedRect(highlight, highlight.height() / 2.0, highlight.height() / 2.0);
    painter.fillPath(shine, QColor(255, 255, 255, 94));

    painter.setPen(QPen(alpha(QColor(255, 255, 255), 184), 1.25));
    painter.drawPath(bubblePath);
    painter.setPen(QPen(QColor(116, 255, 226, 90), 1.0));
    painter.drawRoundedRect(bubble.adjusted(3, 3, -3, -3), radius - 3, radius - 3);
}

void XylarNavBar::paintItems(QPainter &painter)
{
    const QFont baseFont(QStringLiteral("Segoe UI"), 10, QFont::DemiBold);
    painter.setFont(baseFont);

    for (int i = 0; i < m_items.size(); ++i) {
        const QRectF rect = itemRect(i);
        const bool selected = i == m_currentIndex;
        const bool hovered = i == m_hoverIndex;
        const QColor textColor = selected || hovered ? QColor(18, 34, 30) : QColor(242, 255, 250, 220);

        const QRect iconRect(rect.center().x() - 12, rect.top() + 11, 24, 24);
        painter.setOpacity(selected || hovered ? 0.98 : 0.76);
        painter.drawPixmap(iconRect, m_items.at(i).icon.pixmap(QSize(24, 24)));
        painter.setOpacity(1.0);

        painter.setPen(textColor);
        painter.drawText(QRectF(rect.left(), rect.top() + 42, rect.width(), 24), Qt::AlignHCenter | Qt::AlignVCenter, m_items.at(i).label);
    }
}

} // namespace xylar
