#include "LocalPeer.h"
#include <QCoreApplication>
#include <QDataStream>
#include <QDir>
#include <QLocalServer>
#include <QLocalSocket>
#include <QRegularExpression>
#include <QTime>
#include "LockedFile.h"

#if defined(Q_OS_WIN)
#include <qt_windows.h>
#include <QLibrary>
typedef BOOL(WINAPI* PProcessIdToSessionId)(DWORD, DWORD*);
static PProcessIdToSessionId pProcessIdToSessionId = 0;
#endif
#if defined(Q_OS_UNIX)
#include <sys/types.h>
#include <unistd.h>
#endif

#include <QCryptographicHash>
#include <chrono>
#include <thread>

static const char* ack = "ack";

ApplicationId ApplicationId::fromTraditionalApp()
{
    QString protoId = QCoreApplication::applicationFilePath();
#if defined(Q_OS_WIN)
    protoId = protoId.toLower();
#endif
    auto prefix = protoId.section(QLatin1Char('/'), -1);
    static const QRegularExpression s_removeChars("[^a-zA-Z]");
    prefix.remove(s_removeChars);
    prefix.truncate(6);
    QByteArray idc = protoId.toUtf8();
    quint16 idNum = qChecksum(idc);
    auto socketName = QLatin1String("pl") + prefix + QLatin1Char('-') + QString::number(idNum, 16).left(12);
#if defined(Q_OS_WIN)
    if (!pProcessIdToSessionId) {
        QLibrary lib("kernel32");
        pProcessIdToSessionId = (PProcessIdToSessionId)lib.resolve("ProcessIdToSessionId");
    }
    if (pProcessIdToSessionId) {
        DWORD sessionId = 0;
        pProcessIdToSessionId(GetCurrentProcessId(), &sessionId);
        socketName += QLatin1Char('-') + QString::number(sessionId, 16);
    }
#else
    socketName += QLatin1Char('-') + QString::number(::getuid(), 16);
#endif
    return ApplicationId(socketName);
}

ApplicationId ApplicationId::fromPathAndVersion(const QString& dataPath, const QString& version)
{
    QCryptographicHash shasum(QCryptographicHash::Algorithm::Sha1);
    QString result = dataPath + QLatin1Char('-') + version;
    shasum.addData(result.toUtf8());
    return ApplicationId(QLatin1String("pl") + QString::fromLatin1(shasum.result().toHex()).left(12));
}

ApplicationId ApplicationId::fromCustomId(const QString& id)
{
    return ApplicationId(QLatin1String("pl") + id);
}

ApplicationId ApplicationId::fromRawString(const QString& id)
{
    return ApplicationId(id);
}

LocalPeer::LocalPeer(QObject* parent, const ApplicationId& appId) : QObject(parent), id(appId)
{
    socketName = id.toString();
    server.reset(new QLocalServer());
    QString lockName = QDir(QDir::tempPath()).absolutePath() + QLatin1Char('/') + socketName + QLatin1String("-lockfile");
    lockFile.reset(new LockedFile(lockName));
    lockFile->open(QIODevice::ReadWrite);
}

LocalPeer::~LocalPeer() {}

ApplicationId LocalPeer::applicationId() const
{
    return id;
}

bool LocalPeer::isClient()
{
    if (lockFile->isLocked())
        return false;

    if (!lockFile->lock(LockedFile::WriteLock, false))
        return true;

    bool res = server->listen(socketName);
#if defined(Q_OS_UNIX)
    // ### Workaround
    if (!res && server->serverError() == QAbstractSocket::AddressInUseError) {
        QLocalServer::removeServer(socketName);
        res = server->listen(socketName);
    }
#endif
    if (!res)
        qWarning("QtSingleCoreApplication: listen on local socket failed, %s", qPrintable(server->errorString()));
    connect(server.get(), &QLocalServer::newConnection, this, &LocalPeer::receiveConnection);
    return false;
}

bool LocalPeer::sendMessage(const QByteArray& message, int timeout)
{
    if (!isClient())
        return false;

    QLocalSocket socket;
    bool connOk = false;
    int tries = 2;
    for (int i = 0; i < tries; i++) {
        // Try twice, in case the other instance is just starting up
        socket.connectToServer(socketName);
        connOk = socket.waitForConnected(timeout / 2);
        if (!connOk && i < (tries - 1)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
    }
    if (!connOk) {
        return false;
    }

    QByteArray uMsg(message);
    QDataStream ds(&socket);

    ds.writeBytes(uMsg.constData(), uMsg.size());
    if (!socket.waitForBytesWritten(timeout)) {
        return false;
    }

    // wait for 'ack'
    if (!socket.waitForReadyRead(timeout)) {
        return false;
    }

    // make sure we got 'ack'
    if (!(socket.read(qstrlen(ack)) == ack)) {
        return false;
    }
    return true;
}

void LocalPeer::receiveConnection()
{
    QLocalSocket* socket = server->nextPendingConnection();
    if (!socket) {
        return;
    }

    while (socket->bytesAvailable() < static_cast<int>(sizeof(quint32))) {
        socket->waitForReadyRead();
    }
    QDataStream ds(socket);
    QByteArray uMsg;
    quint32 remaining;
    ds >> remaining;
    uMsg.resize(remaining);
    int got = 0;
    char* uMsgBuf = uMsg.data();
    do {
        got = ds.readRawData(uMsgBuf, remaining);
        remaining -= got;
        uMsgBuf += got;
    } while (remaining && got >= 0 && socket->waitForReadyRead(2000));
    if (got < 0) {
        qWarning("QtLocalPeer: Message reception failed %s", socket->errorString().toLatin1().constData());
        delete socket;
        return;
    }
    socket->write(ack, qstrlen(ack));
    socket->waitForBytesWritten(1000);
    socket->waitForDisconnected(1000);  // make sure client reads ack
    delete socket;
    emit messageReceived(uMsg);  // ### (might take a long time to return)
}
