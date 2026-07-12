#include <QTest>
#include <QTimer>

#include <FileSystem.h>

#include <minecraft/mod/ShaderPack.h>
#include <minecraft/mod/tasks/LocalShaderPackParseTask.h>

class ShaderPackParseTest : public QObject {
    Q_OBJECT

   private slots:
    void test_parseZIP()
    {
        QString source = QFINDTESTDATA("testdata/ShaderPackParse");

        QString zip_sp = FS::PathCombine(source, "shaderpack1.zip");
        ShaderPack pack{ QFileInfo(zip_sp) };

        bool valid = ShaderPackUtils::processZIP(pack);

        QVERIFY(pack.packFormat() == ShaderPackFormat::VALID);
        QVERIFY(valid == true);
    }

    void test_parseFolder()
    {
        QString source = QFINDTESTDATA("testdata/ShaderPackParse");

        QString folder_sp = FS::PathCombine(source, "shaderpack2");
        ShaderPack pack{ QFileInfo(folder_sp) };

        bool valid = ShaderPackUtils::processFolder(pack);

        QVERIFY(pack.packFormat() == ShaderPackFormat::VALID);
        QVERIFY(valid == true);
    }

    void test_parseZIP2()
    {
        QString source = QFINDTESTDATA("testdata/ShaderPackParse");

        QString folder_sp = FS::PathCombine(source, "shaderpack3.zip");
        ShaderPack pack{ QFileInfo(folder_sp) };

        bool valid = ShaderPackUtils::process(pack);

        QVERIFY(pack.packFormat() == ShaderPackFormat::INVALID);
        QVERIFY(valid == false);
    }
};

QTEST_GUILESS_MAIN(ShaderPackParseTest)

#include "ShaderPackParse_test.moc"
