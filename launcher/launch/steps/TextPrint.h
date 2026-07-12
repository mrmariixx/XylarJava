#pragma once

#include <LoggedProcess.h>
#include <java/JavaChecker.h>
#include <launch/LaunchStep.h>

/*
 * FIXME: maybe do not export
 */

class TextPrint : public LaunchStep {
    Q_OBJECT
   public:
    explicit TextPrint(LaunchTask* parent, const QStringList& lines, MessageLevel level);
    explicit TextPrint(LaunchTask* parent, const QString& line, MessageLevel level);
    virtual ~TextPrint() {};

    virtual void executeTask();
    virtual bool canAbort() const;
    virtual bool abort();

   private:
    QStringList m_lines;
    MessageLevel m_level;
};
