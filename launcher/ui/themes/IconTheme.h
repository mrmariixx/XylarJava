#pragma once

#include <QString>

class IconTheme {
   public:
    IconTheme(const QString& id, const QString& path) : m_id(id), m_path(path) {}
    IconTheme() = default;

    bool load();
    QString id() const { return m_id; }
    QString path() const { return m_path; }
    QString name() const { return m_name; }

   private:
    QString m_id;
    QString m_path;
    QString m_name;
};
