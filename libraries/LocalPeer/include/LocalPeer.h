#pragma once
#include <QObject>
#include <QString>
#include <memory>

class QLocalServer;
class LockedFile;

class ApplicationId {
   public: /* methods */
    // traditional app = installed system wide and used in a multi-user environment
    static ApplicationId fromTraditionalApp();
    // ID based on a path with all the application data (no two instances with the same data path should run)
    static ApplicationId fromPathAndVersion(const QString& dataPath, const QString& version);
    // custom ID
    static ApplicationId fromCustomId(const QString& id);
    // custom ID, based on a raw string previously acquired from 'toString'
    static ApplicationId fromRawString(const QString& id);

    QString toString() { return m_id; }

   private: /* methods */
    ApplicationId(const QString& value) { m_id = value; }

   private: /* data */
    QString m_id;
};

class LocalPeer : public QObject {
    Q_OBJECT

   public:
    LocalPeer(QObject* parent, const ApplicationId& appId);
    ~LocalPeer();
    bool isClient();
    bool sendMessage(const QByteArray& message, int timeout);
    ApplicationId applicationId() const;

   Q_SIGNALS:
    void messageReceived(const QByteArray& message);

   protected Q_SLOTS:
    void receiveConnection();

   protected:
    ApplicationId id;
    QString socketName;
    std::unique_ptr<QLocalServer> server;
    std::unique_ptr<LockedFile> lockFile;
};
