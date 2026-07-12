#pragma once

#include <QDebug>
#include <QList>
#include <QString>
#include <QStringView>

// this implements the FlexVer
// https://git.sleeping.town/exa/FlexVer
class Version {
   public:
    Version(QString str) : m_string(std::move(str)) { parse(); }  // NOLINT(hicpp-explicit-conversions)
    Version() = default;

   private:
    struct Section {
        enum class Type : std::uint8_t { Null, Textual, Numeric, PreRelease };
        explicit Section(Type t = Type::Null, QString value = "") : t(t), value(std::move(value)) {}
        Type t;
        QString value;
        bool operator==(const Section& other) const = default;
        std::strong_ordering operator<=>(const Section& other) const;
    };

   private:
    void parse();

   public:
    QString toString() const { return m_string; }
    bool isEmpty() const { return m_string.isEmpty(); }

    friend QDebug operator<<(QDebug debug, const Version& v);

    bool operator==(const Version& other) const { return (*this <=> other) == std::strong_ordering::equal; }
    std::strong_ordering operator<=>(const Version& other) const;

   private:
    QString m_string;
    QList<Section> m_sections;
};