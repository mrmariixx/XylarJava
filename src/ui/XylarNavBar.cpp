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
    m_bubbleAnimation->setDuration(460);
    m_bubbleAnimation->setEasingCurve(QEasingCurve::OutExpo);

    m_pulseAnimation = new QPropertyAnimation(this, "pulse", this);
    m_pulseAnimation->setDuration(2200);
    m_pulseAnimation->setStartValue(0.0);
    m_pulseAnimation->setEndValue(1.0);
    m_pulseAnimation->setEasingCurve(QEasingCurve::InOutSine);
    m_pulseAnimation->setLoopCount(-1);
    m_pulseAnimation->start();

    connect(&m_tick, &QTimer::timeout, this, [this]() {
        m_phase += 0.036;
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
    return QRectF(10, 10, width() - 20, height() - 20);
}

QRectF XylarNavBar::itemRect(int index) const
{
    if (m_items.isEmpty() || index < 0 || index >= m_items.size()) {
        return QRectF();
    }

    const QRectF inner = capsuleRect().adjusted(12, 9, -12, -9);
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
    shadow.addRoundedRect(rect.adjusted(0, 11, 0, 17), radius + 5, radius + 5);
    painter.fillPath(shadow, QColor(0, 0, 0, 150));

    QPainterPath shell;
    shell.addRoundedRect(rect, radius, radius);

    QLinearGradient base(rect.topLeft(), rect.bottomRight());
    base.setColorAt(0.0, QColor(255, 255, 255, 72));
    base.setColorAt(0.34, QColor(255, 255, 255, 28));
    base.setColorAt(0.62, QColor(255, 255, 255, 12));
    base.setColorAt(1.0, QColor(255, 255, 255, 6));
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

    QPainterPath lowerMeniscus;
    const QRectF lowerRect = rect.adjusted(22, rect.height() * 0.58, -22, 9);
    lowerMeniscus.addRoundedRect(lowerRect, lowerRect.height() / 2.0, lowerRect.height() / 2.0);
    QLinearGradient lower(lowerRect.topLeft(), lowerRect.bottomLeft());
    lower.setColorAt(0.0, QColor(0, 0, 0, 0));
    lower.setColorAt(1.0, QColor(0, 0, 0, 72));
    painter.fillPath(lowerMeniscus, lower);

    painter.save();
    painter.setClipPath(shell);
    const qreal wave = qSin(m_phase) * 4.0;
    for (int i = 0; i < 3; ++i) {
        const qreal y = rect.top() + rect.height() * (0.26 + i * 0.16);
        QPainterPath lensLine;
        lensLine.moveTo(rect.left() + 30, y + wave * (i + 1) * 0.22);
        lensLine.cubicTo(rect.left() + rect.width() * 0.32,
                         y - 10 + wave,
                         rect.left() + rect.width() * 0.66,
                         y + 12 - wave,
                         rect.right() - 30,
                         y + wave * 0.4);
        painter.setPen(QPen(QColor(255, 255, 255, 14 - i * 3), 1.0));
        painter.drawPath(lensLine);
    }
    painter.restore();

    painter.setPen(QPen(QColor(255, 255, 255, 92), 1.15));
    painter.drawPath(shell);
    painter.setPen(QPen(QColor(255, 255, 255, 26), 1.0));
    painter.drawRoundedRect(rect.adjusted(5, 5, -5, -5), radius - 5, radius - 5);
}

void XylarNavBar::paintBubble(QPainter &painter, const QRectF &rect)
{
    const qreal wave = (qSin(m_phase) + 1.0) * 0.5;
    const qreal pull = m_hoverIndex >= 0 ? qBound(-1.0, (m_mousePosition.x() - rect.center().x()) / qMax(1.0, rect.width()), 1.0) : 0.0;
    const qreal stretch = m_hoverIndex >= 0 ? 10.0 + wave * 4.0 : 4.0;
    const QRectF bubble = rect.adjusted(-stretch + qMin<qreal>(0.0, pull) * 10.0,
                                        -6 - wave * 2.0,
                                        stretch + qMax<qreal>(0.0, pull) * 10.0,
                                        7 + wave * 2.0);
    const qreal radius = bubble.height() / 2.0;
    const QPointF focus = m_hoverIndex >= 0 ? m_mousePosition : bubble.center();

    QPainterPath contactShadow;
    contactShadow.addRoundedRect(bubble.adjusted(8, bubble.height() * 0.58, -8, 11), radius, radius);
    painter.fillPath(contactShadow, QColor(0, 0, 0, 55));

    QPainterPath aura;
    aura.addRoundedRect(bubble.adjusted(-14, -10, 14, 13), radius + 15, radius + 15);
    painter.fillPath(aura, QColor(255, 255, 255, 15 + static_cast<int>(m_pulse * 15)));

    QPainterPath viscosity;
    viscosity.addRoundedRect(bubble.adjusted(-7 - qAbs(pull) * 8.0,
                                             -2,
                                             7 + qAbs(pull) * 8.0,
                                             3),
                             radius + 6,
                             radius + 6);
    painter.fillPath(viscosity, QColor(255, 255, 255, 18));

    QPainterPath bubblePath;
    const qreal left = bubble.left();
    const qreal right = bubble.right();
    const qreal top = bubble.top();
    const qreal bottom = bubble.bottom();
    const qreal centerY = bubble.center().y();
    const qreal bulge = 4.0 + wave * 2.0;
    bubblePath.moveTo(left + radius, top);
    bubblePath.cubicTo(left + bubble.width() * 0.34, top - 2.0 - wave,
                       right - bubble.width() * 0.30, top + 1.0 + pull * 2.0,
                       right - radius, top);
    bubblePath.cubicTo(right + bulge + pull * 4.0, top,
                       right + bulge + pull * 6.0, bottom,
                       right - radius, bottom);
    bubblePath.cubicTo(right - bubble.width() * 0.30, bottom + 2.0 + wave,
                       left + bubble.width() * 0.34, bottom - 1.0 - pull * 2.0,
                       left + radius, bottom);
    bubblePath.cubicTo(left - bulge + pull * 4.0, bottom,
                       left - bulge + pull * 3.0, top,
                       left + radius, top);
    bubblePath.closeSubpath();

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

    painter.save();
    painter.setClipPath(bubblePath);
    for (int i = 0; i < 5; ++i) {
        const qreal phaseOffset = m_phase + i * 0.72;
        const qreal lane = (i + 1) / 6.0;
        QPainterPath caustic;
        caustic.moveTo(bubble.left() + bubble.width() * 0.12,
                       centerY + qSin(phaseOffset) * 9.0 + (lane - 0.5) * bubble.height() * 0.46);
        caustic.cubicTo(bubble.left() + bubble.width() * 0.34,
                        centerY - 18.0 + qCos(phaseOffset) * 7.0,
                        bubble.left() + bubble.width() * 0.66,
                        centerY + 18.0 - qSin(phaseOffset) * 7.0,
                        bubble.right() - bubble.width() * 0.12,
                        centerY + qCos(phaseOffset) * 9.0 + (0.5 - lane) * bubble.height() * 0.38);
        painter.setPen(QPen(QColor(255, 255, 255, 38 - i * 4), 1.15));
        painter.drawPath(caustic);
    }

    QLinearGradient prism(bubble.topLeft(), bubble.topRight());
    prism.setColorAt(0.00, QColor(255, 255, 255, 0));
    prism.setColorAt(0.28, QColor(160, 220, 255, 26));
    prism.setColorAt(0.50, QColor(255, 255, 255, 0));
    prism.setColorAt(0.72, QColor(255, 238, 170, 24));
    prism.setColorAt(1.00, QColor(255, 255, 255, 0));
    painter.fillRect(bubble.adjusted(8, 10, -8, -10), prism);

    QRadialGradient leftLens(QPointF(bubble.left() + 8, centerY), bubble.height() * 0.9);
    leftLens.setColorAt(0.0, QColor(255, 255, 255, 72));
    leftLens.setColorAt(0.46, QColor(255, 255, 255, 18));
    leftLens.setColorAt(1.0, QColor(255, 255, 255, 0));
    painter.fillRect(QRectF(bubble.left(), bubble.top(), bubble.width() * 0.28, bubble.height()), leftLens);

    QRadialGradient rightLens(QPointF(bubble.right() - 8, centerY), bubble.height() * 0.9);
    rightLens.setColorAt(0.0, QColor(0, 0, 0, 42));
    rightLens.setColorAt(0.58, QColor(0, 0, 0, 10));
    rightLens.setColorAt(1.0, QColor(0, 0, 0, 0));
    painter.fillRect(QRectF(bubble.right() - bubble.width() * 0.28, bubble.top(), bubble.width() * 0.28, bubble.height()), rightLens);
    painter.restore();

    const QRectF highlight = bubble.adjusted(18, 8, -18, -bubble.height() * 0.6);
    QPainterPath shine;
    shine.addRoundedRect(highlight, highlight.height() / 2.0, highlight.height() / 2.0);
    painter.fillPath(shine, QColor(255, 255, 255, 118));

    const QRectF glint = QRectF(bubble.right() - bubble.width() * 0.28 + pull * 8.0,
                                bubble.top() + 11,
                                bubble.width() * 0.13,
                                bubble.height() * 0.24);
    QPainterPath glintPath;
    glintPath.addEllipse(glint);
    painter.fillPath(glintPath, QColor(255, 255, 255, 92));

    QPainterPath bottomLens;
    bottomLens.moveTo(bubble.left() + bubble.width() * 0.20, bubble.bottom() - 12);
    bottomLens.cubicTo(bubble.left() + bubble.width() * 0.44,
                       bubble.bottom() - 3,
                       bubble.left() + bubble.width() * 0.66,
                       bubble.bottom() - 3,
                       bubble.right() - bubble.width() * 0.20,
                       bubble.bottom() - 13);
    painter.setPen(QPen(QColor(0, 0, 0, 46), 1.2));
    painter.drawPath(bottomLens);

    painter.setPen(QPen(alpha(QColor(255, 255, 255), 196), 1.35));
    painter.drawPath(bubblePath);
    painter.setPen(QPen(QColor(255, 255, 255, 92), 1.0));
    painter.drawPath(bubblePath.translated(0.0, 1.0));
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
