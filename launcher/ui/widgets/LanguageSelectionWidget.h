#pragma once

#include <QWidget>

class QVBoxLayout;
class QTreeView;
class QLabel;
class Setting;
class QCheckBox;

class LanguageSelectionWidget : public QWidget {
    Q_OBJECT
   public:
    explicit LanguageSelectionWidget(QWidget* parent = 0);
    virtual ~LanguageSelectionWidget() {};

    QString getSelectedLanguageKey() const;
    void retranslate();

   protected slots:
    void languageRowChanged(const QModelIndex& current, const QModelIndex& previous);
    void languageSettingChanged(const Setting&, const QVariant&);

   private:
    QVBoxLayout* verticalLayout = nullptr;
    QTreeView* languageView = nullptr;
    QLabel* helpUsLabel = nullptr;
    QCheckBox* formatCheckbox = nullptr;
};
