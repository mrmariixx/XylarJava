#include "AnonymizeLog.h"

#include <QRegularExpression>

struct RegReplace {
    RegReplace(QRegularExpression r, QString w) : reg(r), with(w) { reg.optimize(); }
    QRegularExpression reg;
    QString with;
};

static const QVector<RegReplace> anonymizeRules = {
    RegReplace(QRegularExpression("C:\\\\Users\\\\([^\\\\]+)\\\\", QRegularExpression::CaseInsensitiveOption),
               "C:\\Users\\********\\"),  // windows
    RegReplace(QRegularExpression("C:\\/Users\\/([^\\/]+)\\/", QRegularExpression::CaseInsensitiveOption),
               "C:/Users/********/"),  // windows with forward slashes
    RegReplace(QRegularExpression("(?<!\\\\w)\\/home\\/[^\\/]+\\/", QRegularExpression::CaseInsensitiveOption),
               "/home/********/"),  // linux
    RegReplace(QRegularExpression("(?<!\\\\w)\\/Users\\/[^\\/]+\\/", QRegularExpression::CaseInsensitiveOption),
               "/Users/********/"),  // macos
    RegReplace(QRegularExpression("\\(Session ID is [^\\)]+\\)", QRegularExpression::CaseInsensitiveOption),
               "(Session ID is <SESSION_TOKEN>)"),  // SESSION_TOKEN
    RegReplace(QRegularExpression("new refresh token: \"[^\"]+\"", QRegularExpression::CaseInsensitiveOption),
               "new refresh token: \"<TOKEN>\""),  // refresh token
    RegReplace(QRegularExpression("\"device_code\" :  \"[^\"]+\"", QRegularExpression::CaseInsensitiveOption),
               "\"device_code\" :  \"<DEVICE_CODE>\""),  // device code
};

void anonymizeLog(QString& log)
{
    for (auto rule : anonymizeRules) {
        log.replace(rule.reg, rule.with);
    }
}
