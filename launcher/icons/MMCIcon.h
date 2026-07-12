#pragma once
#include <QDateTime>
#include <QIcon>
#include <QString>

enum IconType : unsigned { Builtin, Transient, FileBased, ICONS_TOTAL, ToBeDeleted };

struct MMCImage {
    QIcon icon;
    QString key;
    QString filename;
    bool present() const { return !icon.isNull() || !key.isEmpty(); }
};

struct MMCIcon {
    QString m_key;
    QString m_name;
    MMCImage m_images[ICONS_TOTAL];
    IconType m_current_type = ToBeDeleted;

    IconType type() const;
    QString name() const;
    bool has(IconType _type) const;
    QIcon icon() const;
    void remove(IconType rm_type);
    void replace(IconType new_type, QIcon icon, QString path = QString());
    void replace(IconType new_type, const QString& key);
    bool isBuiltIn() const;
    QString getFilePath() const;
};
