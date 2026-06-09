#include <QApplication>
#include <QIcon>

#include "ui/MainWindow.h"
#include "util/Logger.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Launcher"));
    app.setOrganizationName(QStringLiteral("Launcher"));
    app.setWindowIcon(QIcon(QStringLiteral(":/icons/app-icon.png")));

    xylar::Logger::installMessageHandler();
    xylar::Logger::info(QStringLiteral("Starting launcher %1").arg(QStringLiteral(XYLARJAVA_VERSION)));

    xylar::MainWindow window;
    window.show();

    return QApplication::exec();
}
