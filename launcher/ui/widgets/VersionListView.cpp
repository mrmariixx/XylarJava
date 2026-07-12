#include "VersionListView.h"
#include <QApplication>
#include <QDrag>
#include <QHeaderView>
#include <QMouseEvent>
#include <QPainter>

VersionListView::VersionListView(QWidget* parent) : QTreeView(parent)
{
    m_emptyString = tr("No versions are currently available.");
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}

void VersionListView::rowsInserted(const QModelIndex& parent, int start, int end)
{
    m_itemCount += end - start + 1;
    updateEmptyViewPort();
    QTreeView::rowsInserted(parent, start, end);
}

void VersionListView::rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end)
{
    m_itemCount -= end - start + 1;
    updateEmptyViewPort();
    QTreeView::rowsInserted(parent, start, end);
}

void VersionListView::setModel(QAbstractItemModel* model)
{
    m_itemCount = model->rowCount();
    updateEmptyViewPort();
    QTreeView::setModel(model);
}

void VersionListView::reset()
{
    if (model()) {
        m_itemCount = model()->rowCount();
    } else {
        m_itemCount = 0;
    }
    updateEmptyViewPort();
    QTreeView::reset();
}

void VersionListView::setEmptyString(QString emptyString)
{
    m_emptyString = emptyString;
    updateEmptyViewPort();
}

void VersionListView::setEmptyErrorString(QString emptyErrorString)
{
    m_emptyErrorString = emptyErrorString;
    updateEmptyViewPort();
}

void VersionListView::setEmptyMode(VersionListView::EmptyMode mode)
{
    m_emptyMode = mode;
    updateEmptyViewPort();
}

void VersionListView::updateEmptyViewPort()
{
#ifndef QT_NO_ACCESSIBILITY
    setAccessibleDescription(currentEmptyString());
#endif /* !QT_NO_ACCESSIBILITY */

    if (!m_itemCount) {
        viewport()->update();
    }
}

void VersionListView::paintEvent(QPaintEvent* event)
{
    if (m_itemCount) {
        QTreeView::paintEvent(event);
    } else {
        paintInfoLabel(event);
    }
}

QString VersionListView::currentEmptyString() const
{
    switch (m_emptyMode) {
        default:
        case VersionListView::String:
            return m_emptyString;
        case VersionListView::ErrorString:
            return m_emptyErrorString;
    }
}

void VersionListView::paintInfoLabel(QPaintEvent* event) const
{
    QString emptyString = currentEmptyString();

    // calculate the rect for the overlay
    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing, true);
    QFont font("sans", 20);
    font.setBold(true);

    QRect bounds = viewport()->geometry();
    bounds.moveTop(0);
    auto innerBounds = bounds;
    innerBounds.adjust(10, 10, -10, -10);

    QColor background = QApplication::palette().color(QPalette::WindowText);
    QColor foreground = QApplication::palette().color(QPalette::Base);
    foreground.setAlpha(190);
    painter.setFont(font);
    auto fontMetrics = painter.fontMetrics();
    auto textRect = fontMetrics.boundingRect(innerBounds, Qt::AlignHCenter | Qt::TextWordWrap, emptyString);
    textRect.moveCenter(bounds.center());

    auto wrapRect = textRect;
    wrapRect.adjust(-10, -10, 10, 10);

    // check if we are allowed to draw in our area
    if (!event->rect().intersects(wrapRect)) {
        return;
    }

    painter.setBrush(QBrush(background));
    painter.setPen(foreground);
    painter.drawRoundedRect(wrapRect, 5.0, 5.0);

    painter.setPen(foreground);
    painter.setFont(font);
    painter.drawText(textRect, Qt::AlignHCenter | Qt::TextWordWrap, emptyString);
}
