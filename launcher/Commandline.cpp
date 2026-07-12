#include "Commandline.h"

/**
 * @file libutil/src/cmdutils.cpp
 */

namespace Commandline {

// commandline splitter
QStringList splitArgs(QString args)
{
    QStringList argv;
    QString current;
    bool escape = false;
    QChar inquotes;
    for (int i = 0; i < args.length(); i++) {
        QChar cchar = args.at(i);

        // \ escaped
        if (escape) {
            current += cchar;
            escape = false;
            // in "quotes"
        } else if (!inquotes.isNull()) {
            if (cchar == '\\')
                escape = true;
            else if (cchar == inquotes)
                inquotes = QChar::Null;
            else
                current += cchar;
            // otherwise
        } else {
            if (cchar == ' ') {
                if (!current.isEmpty()) {
                    argv << current;
                    current.clear();
                }
            } else if (cchar == '"' || cchar == '\'')
                inquotes = cchar;
            else
                current += cchar;
        }
    }
    if (!current.isEmpty())
        argv << current;
    return argv;
}
}  // namespace Commandline
