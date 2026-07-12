#include "VisualGroup.h"

#include <QApplication>
#include <QDebug>
#include <QModelIndex>
#include <QPainter>
#include <QtMath>
#include <utility>

#include "InstanceView.h"

VisualGroup::VisualGroup(QString text, InstanceView* view) : view(view), text(std::move(text)), collapsed(false) {}

VisualGroup::VisualGroup(const VisualGroup* other) : view(other->view), text(other->text), collapsed(other->collapsed) {}

void VisualGroup::update()
{
    auto temp_items = items();
    auto itemsPerRow = view->itemsPerRow();

    int numRows = qMax(1, qCeil((qreal)temp_items.size() / (qreal)itemsPerRow));
    rows = QList<VisualRow>(numRows);

    int maxRowHeight = 0;
    int positionInRow = 0;
    int currentRow = 0;
    int offsetFromTop = 0;
    for (auto item : temp_items) {
        if (positionInRow == itemsPerRow) {
            rows[currentRow].height = maxRowHeight;
            rows[currentRow].top = offsetFromTop;
            currentRow++;
            if (currentRow >= rows.size()) {
                currentRow = rows.size() - 1;
            }
            offsetFromTop += maxRowHeight + 5;
            positionInRow = 0;
            maxRowHeight = 0;
        }
        QStyleOptionViewItem viewItemOption;
        view->initViewItemOption(&viewItemOption);

        auto itemHeight = view->itemDelegate()->sizeHint(viewItemOption, item).height();
        if (itemHeight > maxRowHeight) {
            maxRowHeight = itemHeight;
        }
        rows[currentRow].items.append(item);
        positionInRow++;
    }
    rows[currentRow].height = maxRowHeight;
    rows[currentRow].top = offsetFromTop;
}

QPair<int, int> VisualGroup::positionOf(const QModelIndex& index) const
{
    int y = 0;
    for (auto& row : rows) {
        for (auto x = 0; x < row.items.size(); x++) {
            if (row.items[x] == index) {
                return qMakePair(x, y);
            }
        }
        y++;
    }
    qWarning() << "Item" << index.row() << index.data(Qt::DisplayRole).toString() << "not found in visual group" << text;
    return qMakePair(0, 0);
}

int VisualGroup::rowTopOf(const QModelIndex& index) const
{
    auto position = positionOf(index);
    return rows[position.second].top;
}

int VisualGroup::rowHeightOf(const QModelIndex& index) const
{
    auto position = positionOf(index);
    return rows[position.second].height;
}

VisualGroup::HitResults VisualGroup::hitScan(const QPoint& pos) const
{
    VisualGroup::HitResults results = VisualGroup::NoHit;
    int y_start = verticalPosition();
    int body_start = y_start + headerHeight();
    int body_end = body_start + contentHeight();
    int y = pos.y();
    // int x = pos.x();
    if (y < y_start) {
        results = VisualGroup::NoHit;
    } else if (y < body_start) {
        results = VisualGroup::HeaderHit;
        int collapseSize = headerHeight() - 4;

        // the icon
        QRect iconRect = QRect(view->m_leftMargin + 2, 2 + y_start, view->width() - 4, collapseSize);
        if (iconRect.contains(pos)) {
            results |= VisualGroup::CheckboxHit;
        }
    } else if (y < body_end) {
        results |= VisualGroup::BodyHit;
    }
    return results;
}

void VisualGroup::drawHeader(QPainter* painter, const QStyleOptionViewItem& option) const
{
    QRect optRect = option.rect;
    optRect.setTop(optRect.top() + 7);
    QFont font(QApplication::font());
    font.setBold(true);
    const QFontMetrics fontMetrics = QFontMetrics(font);
    painter->setFont(font);

    QPen pen;
    pen.setWidth(2);
    QColor penColor = option.palette.text().color();
    penColor.setAlphaF(0.6f);
    pen.setColor(penColor);
    painter->setPen(pen);
    painter->setRenderHint(QPainter::Antialiasing);

    // sizes and offsets, to keep things consistent below
    const int arrowOffsetLeft = fontMetrics.height() / 2 + 7;
    const int textOffsetLeft = arrowOffsetLeft * 2;
    const int centerHeight = optRect.top() + fontMetrics.height() / 2;
    const QString& textToDraw = text.isEmpty() ? QObject::tr("Ungrouped") : text;

    // BEGIN: arrow
    {
        constexpr int arrowSize = 6;
        QPolygon arrowPolygon;
        if (collapsed) {
            arrowPolygon << QPoint(arrowOffsetLeft - arrowSize / 2, centerHeight - arrowSize)
                         << QPoint(arrowOffsetLeft + arrowSize / 2, centerHeight)
                         << QPoint(arrowOffsetLeft - arrowSize / 2, centerHeight + arrowSize);
            painter->drawPolyline(arrowPolygon);
        } else {
            arrowPolygon << QPoint(arrowOffsetLeft - arrowSize, centerHeight - arrowSize / 2)
                         << QPoint(arrowOffsetLeft, centerHeight + arrowSize / 2)
                         << QPoint(arrowOffsetLeft + arrowSize, centerHeight - arrowSize / 2);
            painter->drawPolyline(arrowPolygon);
        }
    }
    // END: arrow

    // BEGIN: text
    {
        QRect textRect(optRect);
        textRect.setTop(textRect.top());
        textRect.setLeft(textOffsetLeft);
        textRect.setHeight(fontMetrics.height());
        textRect.setRight(textRect.right() - 7);

        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, textToDraw);
    }
    // END: text

    // BEGIN: horizontal line
    {
        penColor.setAlphaF(0.05f);
        pen.setColor(penColor);
        painter->setPen(pen);
        // startPoint is left + arrow + text + space
        const int startPoint =
            optRect.left() + fontMetrics.height() + fontMetrics.size(Qt::AlignLeft | Qt::AlignVCenter, textToDraw).width() + 20;
        painter->setRenderHint(QPainter::Antialiasing, false);
        QPolygon polygon;
        // for some reason the height (yPos) doesn't look centered, so we are adding 1 to the center height
        const int lineHeight = centerHeight + 1;
        polygon << QPoint(startPoint, lineHeight) << QPoint(optRect.right() - 3, lineHeight);
        painter->drawPolyline(polygon);
    }
    // END: horizontal line
}

int VisualGroup::totalHeight() const
{
    return headerHeight() + contentHeight();
}

int VisualGroup::headerHeight()
{
    QFont font(QApplication::font());
    font.setBold(true);
    QFontMetrics fontMetrics(font);

    const int height = fontMetrics.height() + 1 /* 1 pixel-width gradient */
                       + 11 /* top and bottom separation */;
    return height;
    /*
    int raw = view->viewport()->fontMetrics().height() + 4;
    // add english. maybe. depends on font height.
    if (raw % 2 == 0)
        raw++;
    return std::min(raw, 25);
    */
}

int VisualGroup::contentHeight() const
{
    if (collapsed) {
        return 0;
    }
    auto last = rows[numRows() - 1];
    return last.top + last.height;
}

int VisualGroup::numRows() const
{
    return (int)rows.size();
}

int VisualGroup::verticalPosition() const
{
    return m_verticalPosition;
}

QList<QModelIndex> VisualGroup::items() const
{
    QList<QModelIndex> indices;
    for (int i = 0; i < view->model()->rowCount(); ++i) {
        const QModelIndex index = view->model()->index(i, 0);
        if (index.data(InstanceViewRoles::GroupRole).toString() == text) {
            indices.append(index);
        }
    }
    return indices;
}
