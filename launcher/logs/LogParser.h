#pragma once
#include <QAnyStringView>
#include <QDateTime>
#include <QList>
#include <QString>
#include <QStringView>
#include <QXmlStreamReader>
#include <optional>
#include <variant>
#include "MessageLevel.h"

class LogParser {
   public:
    struct LogEntry {
        QString logger;
        MessageLevel level;
        QString levelText;
        QDateTime timestamp;
        QString thread;
        QString message;
    };
    struct Partial {
        QString data;
    };
    struct PlainText {
        QString message;
    };
    struct Error {
        QString errMessage;
        QXmlStreamReader::Error error;
    };

    using ParsedItem = std::variant<LogEntry, PlainText, Partial>;

   public:
    LogParser() = default;

    void appendLine(QAnyStringView data);
    std::optional<ParsedItem> parseNext();
    QList<ParsedItem> parseAvailable();
    std::optional<Error> getError();

    /// guess log level from a line of game log
    static MessageLevel guessLevel(const QString& line, MessageLevel previous);

   protected:
    std::optional<LogEntry> parseAttributes();
    void setError();
    void clearError();

    std::optional<ParsedItem> parseLog4J();

   private:
    QString m_buffer;
    QString m_partialData;
    QXmlStreamReader m_parser;
    std::optional<Error> m_error;
};
