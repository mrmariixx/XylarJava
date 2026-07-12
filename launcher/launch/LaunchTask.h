#pragma once
#include <QObjectPtr.h>
#include <minecraft/MinecraftInstance.h>
#include <QProcess>
#include "LaunchStep.h"
#include "LogModel.h"
#include "MessageLevel.h"
#include "logs/LogParser.h"

class LaunchTask : public Task {
    Q_OBJECT
   protected:
    explicit LaunchTask(MinecraftInstance* instance);
    void init();

   public:
    enum State { NotStarted, Running, Waiting, Failed, Aborted, Finished };

   public: /* methods */
    static std::unique_ptr<LaunchTask> create(MinecraftInstance* inst);
    virtual ~LaunchTask() = default;

    void appendStep(shared_qobject_ptr<LaunchStep> step);
    void prependStep(shared_qobject_ptr<LaunchStep> step);
    void setCensorFilter(QMap<QString, QString> filter);

    MinecraftInstance* instance() { return m_instance; }

    void setPid(qint64 pid) { m_pid = pid; }

    qint64 pid() { return m_pid; }

    /**
     * @brief prepare the process for launch (for multi-stage launch)
     */
    virtual void executeTask() override;

    /**
     * @brief launch the armed instance
     */
    void proceed();

    /**
     * @brief abort launch
     */
    bool abort() override;

    bool canAbort() const override;

    shared_qobject_ptr<LogModel> getLogModel();

   public:
    QString substituteVariables(QString& cmd, bool isLaunch = false) const;
    QString censorPrivateInfo(QString in);

   protected: /* methods */
    virtual void emitFailed(QString reason) override;
    virtual void emitSucceeded() override;

   signals:
    /**
     * @brief emitted when the launch preparations are done
     */
    void readyForLaunch();

    void requestProgress(Task* task);

    void requestLogging();

   public slots:
    void onLogLines(const QStringList& lines, MessageLevel defaultLevel = MessageLevel::Launcher);
    void onLogLine(QString line, MessageLevel defaultLevel = MessageLevel::Launcher);
    void onReadyForLaunch();
    void onStepFinished();
    void onProgressReportingRequested();

   private: /*methods */
    void finalizeSteps(bool successful, const QString& error);

   protected:
    bool parseXmlLogs(QString const& line, MessageLevel level);

   protected: /* data */
    MinecraftInstance* m_instance;
    shared_qobject_ptr<LogModel> m_logModel;
    QList<shared_qobject_ptr<LaunchStep>> m_steps;
    QMap<QString, QString> m_censorFilter;
    int currentStep = -1;
    State state = NotStarted;
    qint64 m_pid = -1;
    LogParser m_stdoutParser;
    LogParser m_stderrParser;
};
