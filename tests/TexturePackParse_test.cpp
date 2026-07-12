#include <QTest>
#include <QTimer>

#include "FileSystem.h"

#include "minecraft/mod/TexturePack.h"
#include "minecraft/mod/tasks/LocalTexturePackParseTask.h"

class TexturePackParseTest : public QObject {
    Q_OBJECT

   private slots:
    void test_parseZIP()
    {
        QString source = QFINDTESTDATA("testdata/TexturePackParse");

        QString zip_rp = FS::PathCombine(source, "test_texture_pack_idk.zip");
        TexturePack pack{ QFileInfo(zip_rp) };

        bool valid = TexturePackUtils::processZIP(pack);

        QVERIFY(pack.description() == "joe biden, wake up");
        QVERIFY(valid == true);
    }

    void test_parseFolder()
    {
        QString source = QFINDTESTDATA("testdata/TexturePackParse");

        QString folder_rp = FS::PathCombine(source, "test_texturefolder");
        TexturePack pack{ QFileInfo(folder_rp) };

        bool valid = TexturePackUtils::processFolder(pack, TexturePackUtils::ProcessingLevel::BasicInfoOnly);

        QVERIFY(pack.description() == "Some texture pack surely");
        QVERIFY(valid == true);
    }

    void test_parseFolder2()
    {
        QString source = QFINDTESTDATA("testdata/TexturePackParse");

        QString folder_rp = FS::PathCombine(source, "another_test_texturefolder");
        TexturePack pack{ QFileInfo(folder_rp) };

        bool valid = TexturePackUtils::process(pack, TexturePackUtils::ProcessingLevel::BasicInfoOnly);

        QVERIFY(pack.description() == "quieres\nfor real");
        QVERIFY(valid == true);
    }
};

QTEST_GUILESS_MAIN(TexturePackParseTest)

#include "TexturePackParse_test.moc"
