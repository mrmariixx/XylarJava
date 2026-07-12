#pragma once

#include "MessageLevel.h"
#include "tasks/Task.h"

#include <QStringList>

class LaunchTask;
class LaunchStep : public Task {
    Q_OBJECT
   public: /* methods */
    explicit LaunchStep(LaunchTask* parent);
    virtual ~LaunchStep() = default;

   signals:
    void logLines(QStringList lines, MessageLevel level);
    void logLine(QString line, MessageLevel level);
    void readyForLaunch();
    void progressReportingRequest();

   public slots:
    virtual void proceed() {};
    // called in the opposite order than the Task launch(), used to clean up or otherwise undo things after the launch ends
    virtual void finalize() {};

   protected: /* data */
    LaunchTask* m_parent;
};
