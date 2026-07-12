#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QTest>
#include <QTimer>

#include <FileSystem.h>

#include <minecraft/mod/tasks/LocalDataPackParseTask.h>

class MetaComponentParseTest : public QObject {
    Q_OBJECT

    void doTest(QString name)
    {
        QString source = QFINDTESTDATA("testdata/MetaComponentParse");

        QString comp_rp = FS::PathCombine(source, name);

        QFile file;
        file.setFileName(comp_rp);
        QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
        QString data = file.readAll();
        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
        QJsonObject obj = doc.object();

        QJsonValue description_json = obj.value("description");
        QJsonValue expected_json = obj.value("expected_output");

        QVERIFY(!description_json.isUndefined());
        QVERIFY(expected_json.isString());

        QString expected = expected_json.toString();

        QString processed = DataPackUtils::processComponent(description_json);

        QCOMPARE(processed, expected);
    }

   private slots:
    void test_parseComponentBasic() { doTest("component_basic.json"); }
    void test_parseComponentWithFormat() { doTest("component_with_format.json"); }
    void test_parseComponentWithExtra() { doTest("component_with_extra.json"); }
    void test_parseComponentWithLink() { doTest("component_with_link.json"); }
    void test_parseComponentWithMixed() { doTest("component_with_mixed.json"); }
};

QTEST_GUILESS_MAIN(MetaComponentParseTest)

#include "MetaComponentParse_test.moc"
