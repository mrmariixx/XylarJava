#include <QTest>
#include <QTimer>
#include "minecraft/mod/tasks/LocalDataPackParseTask.h"

#include <FileSystem.h>

#include <minecraft/mod/ResourcePack.h>

class ResourcePackParseTest : public QObject {
    Q_OBJECT

   private slots:
    void test_parseZIP()
    {
        QString source = QFINDTESTDATA("testdata/Resources");

        QString zip_rp = FS::PathCombine(source, "test_resource_pack_idk.zip");
        ResourcePack pack{ QFileInfo(zip_rp) };

        bool valid = DataPackUtils::processZIP(&pack, DataPackUtils::ProcessingLevel::BasicInfoOnly);

        QVERIFY(pack.packFormat() == 3);
        QVERIFY(pack.description() ==
                "um dois, feijão com arroz, três quatro, feijão no prato, cinco seis, café inglês, sete oito, comer biscoito, nove dez "
                "comer pastéis!!");
        QVERIFY(valid == true);
    }

    void test_parseFolder()
    {
        QString source = QFINDTESTDATA("testdata/Resources");

        QString folder_rp = FS::PathCombine(source, "test_folder");
        ResourcePack pack{ QFileInfo(folder_rp) };

        bool valid = DataPackUtils::processFolder(&pack, DataPackUtils::ProcessingLevel::BasicInfoOnly);

        QVERIFY(pack.packFormat() == 1);
        QVERIFY(pack.description() == "Some resource pack maybe");
        QVERIFY(valid == true);
    }

    void test_parseFolder2()
    {
        QString source = QFINDTESTDATA("testdata/Resources");

        QString folder_rp = FS::PathCombine(source, "another_test_folder");
        ResourcePack pack{ QFileInfo(folder_rp) };

        bool valid = DataPackUtils::process(&pack, DataPackUtils::ProcessingLevel::BasicInfoOnly);

        QVERIFY(pack.packFormat() == 6);
        QVERIFY(pack.description() == "o quartel pegou fogo, policia deu sinal, acode acode acode a bandeira nacional");
        QVERIFY(valid == true);  // no assets dir but it is still valid based on https://minecraft.wiki/w/Resource_pack
    }
};

QTEST_GUILESS_MAIN(ResourcePackParseTest)

#include "ResourcePackParse_test.moc"
