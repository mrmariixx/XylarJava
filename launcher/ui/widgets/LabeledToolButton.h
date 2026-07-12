#pragma once

#include <QPushButton>
#include <QToolButton>

class QLabel;

class LabeledToolButton : public QToolButton {
    Q_OBJECT

    QLabel* m_label;
    QIcon m_icon;

   public:
    LabeledToolButton(QWidget* parent = 0);

    QString text() const;
    void setText(const QString& text);
    void setIcon(QIcon icon);
    virtual QSize sizeHint() const;

   protected:
    void resizeEvent(QResizeEvent* event);
    void resetIcon();
};
