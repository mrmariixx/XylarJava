#pragma once

#include <LoggedProcess.h>
#include <java/JavaChecker.h>
#include <launch/LaunchStep.h>

class CheckJava : public LaunchStep {
    Q_OBJECT
   public:
    explicit CheckJava(LaunchTask* parent) : LaunchStep(parent) {};
    virtual ~CheckJava() = default;

    virtual void executeTask();
    virtual bool canAbort() const { return false; }
   private slots:
    void checkJavaFinished(const JavaChecker::Result& result);

   private:
    void printJavaInfo(const QString& version, const QString& architecture, const QString& realArchitecture, const QString& vendor);
    void printSystemInfo(bool javaIsKnown, bool javaIs64bit);

   private:
    QString m_javaPath;
    QString m_javaSignature;
    JavaChecker::Ptr m_JavaChecker;
};
