#include <QApplication>
#include <QIcon>

#include "ui/MainWindow.h"
#include "util/Logger.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("XylarJava"));
    app.setOrganizationName(QStringLiteral("Xylar"));
    app.setWindowIcon(QIcon(QStringLiteral(":/icons/app-icon.png")));

    xylar::Logger::installMessageHandler();
    xylar::Logger::info(QStringLiteral("Starting XylarJava %1").arg(QStringLiteral(XYLARJAVA_VERSION)));

    xylar::MainWindow window;
    window.show();

    return QApplication::exec();
}
