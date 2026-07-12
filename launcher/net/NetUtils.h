#pragma once

#include <QNetworkReply>
#include <QSet>

namespace Net {
inline bool isApplicationError(QNetworkReply::NetworkError x)
{
    // Mainly taken from https://github.com/qt/qtbase/blob/dev/src/network/access/qhttpthreaddelegate.cpp
    static QSet<QNetworkReply::NetworkError> errors = { QNetworkReply::ProtocolInvalidOperationError,
                                                        QNetworkReply::AuthenticationRequiredError,
                                                        QNetworkReply::ContentAccessDenied,
                                                        QNetworkReply::ContentNotFoundError,
                                                        QNetworkReply::ContentOperationNotPermittedError,
                                                        QNetworkReply::ProxyAuthenticationRequiredError,
                                                        QNetworkReply::ContentConflictError,
                                                        QNetworkReply::ContentGoneError,
                                                        QNetworkReply::InternalServerError,
                                                        QNetworkReply::OperationNotImplementedError,
                                                        QNetworkReply::ServiceUnavailableError,
                                                        QNetworkReply::UnknownServerError,
                                                        QNetworkReply::UnknownContentError };
    return errors.contains(x);
}
}  // namespace Net
