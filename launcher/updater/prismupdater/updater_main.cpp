#include "PrismUpdater.h"

#if defined Q_OS_WIN32
#include "console/WindowsConsole.h"
#endif

int main(int argc, char* argv[])
{
#if defined Q_OS_WIN32
    // attach the parent console if stdout not already captured
    console::WindowsConsoleGuard _consoleGuard;
#endif

    PrismUpdaterApp wUpApp(argc, argv);

    switch (wUpApp.status()) {
        case PrismUpdaterApp::Starting:
        case PrismUpdaterApp::Initialized: {
            return wUpApp.exec();
        }
        case PrismUpdaterApp::Failed:
            return 1;
        case PrismUpdaterApp::Succeeded:
            return 0;
        default:
            return -1;
    }
}
