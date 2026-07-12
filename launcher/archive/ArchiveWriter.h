#pragma once

#include <QByteArray>
#include <QFileDevice>
#include "archive/ArchiveReader.h"

struct archive;
namespace MMCZip {

class ArchiveWriter {
   public:
    ArchiveWriter(const QString& archiveName);
    virtual ~ArchiveWriter();

    bool open();
    bool close();

    bool addFile(const QString& fileName, const QString& fileDest);
    bool addFile(const QString& fileDest, const QByteArray& data);
    bool addFile(ArchiveReader::File* f);

    static std::unique_ptr<archive, void (*)(archive*)> createDiskWriter();

   private:
    struct archive* m_archive = nullptr;
    QString m_filename;
    QString m_format = "zip";
};
}  // namespace MMCZip
