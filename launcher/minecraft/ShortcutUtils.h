#pragma once
#include "Application.h"
#include "BaseInstance.h"

#include <QList>
#include <QMessageBox>

namespace ShortcutUtils {
/// A struct to hold parameters for creating a shortcut
struct Shortcut {
    BaseInstance* instance;
    QString name;
    QString targetString;
    QWidget* parent = nullptr;
    QStringList extraArgs = {};
    QString iconKey = "";
    ShortcutTarget target;
};

/// Create an instance shortcut on the specified file path
bool createInstanceShortcut(const Shortcut& shortcut, const QString& filePath);

/// Create an instance shortcut on the desktop
bool createInstanceShortcutOnDesktop(const Shortcut& shortcut);

/// Create an instance shortcut in the Applications directory
bool createInstanceShortcutInApplications(const Shortcut& shortcut);

/// Create an instance shortcut in other directories
bool createInstanceShortcutInOther(const Shortcut& shortcut);

}  // namespace ShortcutUtils
