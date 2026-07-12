#pragma once

#include <QFile>
#ifdef Q_OS_WIN
#include <QList>
#endif

class LockedFile : public QFile {
   public:
    enum LockMode { NoLock = 0, ReadLock, WriteLock };

    LockedFile();
    LockedFile(const QString& name);
    ~LockedFile();

    bool open(OpenMode mode);

    bool lock(LockMode mode, bool block = true);
    bool unlock();
    bool isLocked() const;
    LockMode lockMode() const;

   private:
#ifdef Q_OS_WIN
    Qt::HANDLE wmutex;
    Qt::HANDLE rmutex;
    QList<Qt::HANDLE> rmutexes;
    QString mutexname;

    Qt::HANDLE getMutexHandle(int idx, bool doCreate);
    bool waitMutex(Qt::HANDLE mutex, bool doBlock);
#endif

    LockMode m_lock_mode;
};
