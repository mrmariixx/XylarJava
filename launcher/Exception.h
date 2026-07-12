#pragma once

#include <QDebug>
#include <QString>
#include <exception>

class Exception : public std::exception {
   public:
    Exception(const QString& message) : std::exception(), m_message(message.toUtf8()) { qCritical() << "Exception:" << message; }
    Exception(const Exception& other) : std::exception(), m_message(other.m_message) {}
    virtual ~Exception() noexcept {}
    const char* what() const noexcept { return m_message.constData(); }
    QString cause() const { return QString::fromUtf8(m_message); }

   private:
    QByteArray m_message;
};
