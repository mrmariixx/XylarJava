#include "AccountTokenCipher.h"

#include <QCryptographicHash>
#include <QSysInfo>

namespace {
constexpr auto kPrefix = "enc1:";

QByteArray deriveKey()
{
    const QByteArray material = (QSysInfo::machineUniqueId() + QStringLiteral("XylarJava-v1")).toUtf8();
    return QCryptographicHash::hash(material, QCryptographicHash::Sha256);
}

QByteArray transform(const QByteArray& input)
{
    const QByteArray key = deriveKey();
    QByteArray out = input;
    for (int i = 0; i < out.size(); ++i) {
        out[i] = static_cast<char>(out[i] ^ key[i % key.size()]);
    }
    return out;
}
}  // namespace

namespace AccountTokenCipher {

bool isEncrypted(const QString& stored)
{
    return stored.startsWith(QLatin1String(kPrefix));
}

QString encrypt(const QString& plain)
{
    if (plain.isEmpty()) {
        return {};
    }
    return QString::fromLatin1(kPrefix) + QString::fromLatin1(transform(plain.toUtf8()).toBase64());
}

QString decrypt(const QString& stored)
{
    if (stored.isEmpty()) {
        return {};
    }
    if (!isEncrypted(stored)) {
        return stored;
    }
    const QByteArray payload = QByteArray::fromBase64(stored.mid(int(sizeof(kPrefix) - 1)).toLatin1());
    return QString::fromUtf8(transform(payload));
}

}  // namespace AccountTokenCipher
