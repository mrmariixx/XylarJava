#pragma once

#include <QIODevice>
#include <QString>
#include <QVariant>

#include <QJsonArray>
#include <QJsonDocument>

// Sectionless INI parser (for instance config files)
class INIFile : public QMap<QString, QVariant> {
   public:
    explicit INIFile();

    bool loadFile(QString fileName);
    bool loadFile(QByteArray data);
    bool saveFile(QString fileName);

    QVariant get(QString key, QVariant def) const;
    void set(QString key, QVariant val);
};
