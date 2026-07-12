#pragma once

#include <QComboBox>
#include <QLineEdit>

class CheckComboBox : public QComboBox {
    Q_OBJECT

   public:
    explicit CheckComboBox(QWidget* parent = nullptr);
    virtual ~CheckComboBox() = default;

    void hidePopup() override;

    QString defaultText() const;
    void setDefaultText(const QString& text);

    Qt::CheckState itemCheckState(int index) const;
    void setItemCheckState(int index, Qt::CheckState state);

    QString separator() const;
    void setSeparator(const QString& separator);

    QStringList checkedItems() const;

    void setSourceModel(QAbstractItemModel* model);

   public slots:
    void setCheckedItems(const QStringList& items);

   signals:
    void checkedItemsChanged(const QStringList& items);

   protected:
    void paintEvent(QPaintEvent*) override;

   private:
    void emitCheckedItemsChanged();
    bool eventFilter(QObject* receiver, QEvent* event) override;
    void toggleCheckState(int index);

   private:
    QString m_default_text;
    QString m_separator;
    bool m_containerMousePress = false;
};
