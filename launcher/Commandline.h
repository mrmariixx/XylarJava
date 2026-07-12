#pragma once

#include <QString>
#include <QStringList>

/**
 * @file libutil/include/cmdutils.h
 * @brief commandline parsing and processing utilities
 */

namespace Commandline {

/**
 * @brief split a string into argv items like a shell would do
 * @param args the argument string
 * @return a QStringList containing all arguments
 */
QStringList splitArgs(QString args);
}  // namespace Commandline
