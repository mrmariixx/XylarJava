#pragma once

#include <QString>

namespace AccountTokenCipher {
QString encrypt(const QString& plain);
QString decrypt(const QString& stored);
bool isEncrypted(const QString& stored);
}  // namespace AccountTokenCipher
