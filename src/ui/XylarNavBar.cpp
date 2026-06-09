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
    m_bubbleAnimation->setDuration(520);
    m_bubbleAnimation->setEasingCurve(QEasingCurve::OutQuart);

    m_pulseAnimation = new QPropertyAnimation(this, "pulse", this);
    m_pulseAnimation->setDuration(1850);
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
    shadow.addRoundedRect(rect.adjusted(0, 9, 0, 15), radius + 3, radius + 3);
    painter.fillPath(shadow, QColor(0, 0, 0, 128));

    QPainterPath shell;
    shell.addRoundedRect(rect, radius, radius);

    QLinearGradient base(rect.topLeft(), rect.bottomRight());
    base.setColorAt(0.0, QColor(255, 255, 255, 58));
    base.setColorAt(0.35, QColor(255, 255, 255, 26));
    base.setColorAt(0.70, QColor(255, 255, 255, 14));
    base.setColorAt(1.0, QColor(255, 255, 255, 8));
    painter.fillPath(shell, base);

    QLinearGradient rim(rect.left(), rect.top(), rect.left(), rect.bottom());
    rim.setColorAt(0.0, QColor(255, 255, 255, 152));
    rim.setColorAt(0.32, QColor(255, 255, 255, 24));
    rim.setColorAt(0.78, QColor(0, 0, 0, 18));
    rim.setColorAt(1.0, QColor(0, 0, 0, 55));
    painter.fillPath(shell, rim);

    QPainterPath topSheen;
    const QRectF sheenRect = rect.adjusted(18, 9, -18, -rect.height() * 0.60);
    topSheen.addRoundedRect(sheenRect, sheenRect.height() / 2.0, sheenRect.height() / 2.0);
    QLinearGradient sheen(sheenRect.topLeft(), sheenRect.topRight());
    sheen.setColorAt(0.0, QColor(255, 255, 255, 12));
    sheen.setColorAt(0.45, QColor(255, 255, 255, 82));
    sheen.setColorAt(1.0, QColor(255, 255, 255, 10));
    painter.fillPath(topSheen, sheen);

    painter.setPen(QPen(QColor(255, 255, 255, 92), 1.15));
    painter.drawPath(shell);
    painter.setPen(QPen(QColor(255, 255, 255, 26), 1.0));
    painter.drawRoundedRect(rect.adjusted(5, 5, -5, -5), radius - 5, radius - 5);
}

void XylarNavBar::paintBubble(QPainter &painter, const QRectF &rect)
{
    const qreal wave = (qSin(m_phase) + 1.0) * 0.5;
    const qreal stretch = m_hoverIndex >= 0 ? 8.0 + wave * 3.0 : 4.0;
    const QRectF bubble = rect.adjusted(-stretch, -5 - wave * 1.5, stretch, 6 + wave * 1.5);
    const qreal radius = bubble.height() / 2.0;
    const QPointF focus = m_hoverIndex >= 0 ? m_mousePosition : bubble.center();

    QPainterPath contactShadow;
    contactShadow.addRoundedRect(bubble.adjusted(8, bubble.height() * 0.58, -8, 11), radius, radius);
    painter.fillPath(contactShadow, QColor(0, 0, 0, 55));

    QPainterPath aura;
    aura.addRoundedRect(bubble.adjusted(-14, -10, 14, 13), radius + 15, radius + 15);
    painter.fillPath(aura, QColor(255, 255, 255, 15 + static_cast<int>(m_pulse * 15)));

    QPainterPath bubblePath;
    bubblePath.addRoundedRect(bubble, radius, radius);

    QRadialGradient fill(focus, bubble.width() * 0.78, focus);
    fill.setColorAt(0.0, QColor(255, 255, 255, 188));
    fill.setColorAt(0.30, QColor(255, 255, 255, 118));
    fill.setColorAt(0.68, QColor(255, 255, 255, 48));
    fill.setColorAt(1.0, QColor(255, 255, 255, 20));
    painter.fillPath(bubblePath, fill);

    QLinearGradient shade(bubble.topLeft(), bubble.bottomLeft());
    shade.setColorAt(0.0, QColor(255, 255, 255, 150));
    shade.setColorAt(0.42, QColor(255, 255, 255, 22));
    shade.setColorAt(1.0, QColor(0, 0, 0, 50));
    painter.fillPath(bubblePath, shade);

    const QRectF highlight = bubble.adjusted(18, 8, -18, -bubble.height() * 0.6);
    QPainterPath shine;
    shine.addRoundedRect(highlight, highlight.height() / 2.0, highlight.height() / 2.0);
    painter.fillPath(shine, QColor(255, 255, 255, 94));

    const QRectF glint = QRectF(bubble.right() - bubble.width() * 0.28, bubble.top() + 11, bubble.width() * 0.13, bubble.height() * 0.24);
    QPainterPath glintPath;
    glintPath.addEllipse(glint);
    painter.fillPath(glintPath, QColor(255, 255, 255, 78));

    painter.setPen(QPen(alpha(QColor(255, 255, 255), 196), 1.35));
    painter.drawPath(bubblePath);
    painter.setPen(QPen(QColor(255, 255, 255, 92), 1.0));
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
        const QColor textColor = selected || hovered ? QColor(5, 5, 5) : QColor(245, 245, 247, 220);

        const QRect iconRect(rect.center().x() - 12, rect.top() + 11, 24, 24);
        QPixmap icon = m_items.at(i).icon.pixmap(QSize(24, 24));
        QPixmap tinted(icon.size());
        tinted.fill(Qt::transparent);
        {
            QPainter iconPainter(&tinted);
            iconPainter.setRenderHint(QPainter::Antialiasing, true);
            iconPainter.drawPixmap(0, 0, icon);
            iconPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
            iconPainter.fillRect(tinted.rect(), selected || hovered ? QColor(5, 5, 5) : QColor(245, 245, 247));
        }
        painter.setOpacity(selected || hovered ? 0.98 : 0.76);
        painter.drawPixmap(iconRect, tinted);
        painter.setOpacity(1.0);

        painter.setPen(textColor);
        painter.drawText(QRectF(rect.left(), rect.top() + 42, rect.width(), 24), Qt::AlignHCenter | Qt::AlignVCenter, m_items.at(i).label);
    }
}

} // namespace xylar
