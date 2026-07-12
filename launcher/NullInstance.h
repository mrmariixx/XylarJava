#pragma once
#include "BaseInstance.h"
#include "launch/LaunchTask.h"

class NullInstance : public BaseInstance {
    Q_OBJECT
   public:
    NullInstance(SettingsObject* globalSettings, std::unique_ptr<SettingsObject> settings, const QString& rootDir)
        : BaseInstance(globalSettings, std::move(settings), rootDir)
    {
        setVersionBroken(true);
    }
    virtual ~NullInstance() = default;
    void saveNow() override {}
    void loadSpecificSettings() override { setSpecificSettingsLoaded(true); }
    QString getStatusbarDescription() override { return tr("Unknown instance type"); };
    QSet<QString> traits() const override { return {}; };
    QString instanceConfigFolder() const override { return instanceRoot(); };
    LaunchTask* createLaunchTask(AuthSessionPtr, MinecraftTarget::Ptr) override { return nullptr; }
    QList<Task::Ptr> createUpdateTask() override { return {}; }
    QProcessEnvironment createEnvironment() override { return QProcessEnvironment(); }
    QProcessEnvironment createLaunchEnvironment() override { return QProcessEnvironment(); }
    QMap<QString, QString> getVariables() override { return QMap<QString, QString>(); }
    QStringList getLogFileSearchPaths() override { return {}; }
    QString typeName() const override { return "Null"; }
    bool canExport() const override { return false; }
    bool canEdit() const override { return false; }
    bool canLaunch() const override { return false; }
    void populateLaunchMenu(QMenu* menu) override {}
    QStringList verboseDescription(AuthSessionPtr session, MinecraftTarget::Ptr targetToJoin) override
    {
        QStringList out;
        out << "Null instance - placeholder.";
        return out;
    }
    QString modsRoot() const override { return QString(); }
    void updateRuntimeContext() override
    {
        // NOOP
    }
};
