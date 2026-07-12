#pragma once

#include <QDate>
#include <QFileInfo>
#include <QList>
#include <QString>

class CatPack {
   public:
    virtual ~CatPack() {}
    virtual QString id() const = 0;
    virtual QString name() const = 0;
    virtual QString path() const = 0;
};

class BasicCatPack : public CatPack {
   public:
    BasicCatPack(QString id, QString name) : m_id(id), m_name(name) {}
    BasicCatPack(QString id) : BasicCatPack(id, id) {}
    virtual QString id() const override { return m_id; }
    virtual QString name() const override { return m_name; }
    virtual QString path() const override;

   protected:
    QString m_id;
    QString m_name;
};

class FileCatPack : public BasicCatPack {
   public:
    FileCatPack(QString id, QFileInfo& fileInfo) : BasicCatPack(id), m_path(fileInfo.absoluteFilePath()) {}
    FileCatPack(QFileInfo& fileInfo) : FileCatPack(fileInfo.baseName(), fileInfo) {}
    virtual QString path() const { return m_path; }

   private:
    QString m_path;
};

class JsonCatPack : public BasicCatPack {
   public:
    struct PartialDate {
        int month;
        int day;
    };
    struct Variant {
        QString path;
        PartialDate startTime;
        PartialDate endTime;
    };
    JsonCatPack(QFileInfo& manifestInfo);
    virtual QString path() const override;
    QString path(QDate now) const;

   private:
    QString m_default_path;
    QList<Variant> m_variants;
};
