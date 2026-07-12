#pragma once

#include "BaseInstance.h"

/// Update instanceRoot to make it sync with name/id; return newRoot if a directory rename happened
QString askToUpdateInstanceDirName(BaseInstance* instance, const QString& oldName, const QString& newName, QWidget* parent);

/// Check if there are linked instances, and display a warning; return true if the operation should proceed
bool checkLinkedInstances(const QString& id, QWidget* parent, const QString& verb);
