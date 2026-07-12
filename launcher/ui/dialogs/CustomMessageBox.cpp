#include "CustomMessageBox.h"

namespace CustomMessageBox {
QMessageBox* selectable(QWidget* parent,
                        const QString& title,
                        const QString& text,
                        QMessageBox::Icon icon,
                        QMessageBox::StandardButtons buttons,
                        QMessageBox::StandardButton defaultButton,
                        QCheckBox* checkBox)
{
    QMessageBox* messageBox = new QMessageBox(parent);
    messageBox->setWindowTitle(title);
    messageBox->setText(text);
    messageBox->setStandardButtons(buttons);
    messageBox->setDefaultButton(defaultButton);
    messageBox->setTextInteractionFlags(Qt::TextSelectableByMouse);
    messageBox->setIcon(icon);
    messageBox->setTextInteractionFlags(Qt::TextBrowserInteraction);
    if (checkBox)
        messageBox->setCheckBox(checkBox);

    return messageBox;
}
}  // namespace CustomMessageBox
