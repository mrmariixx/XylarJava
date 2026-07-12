#include "FileLink.h"

#if defined Q_OS_WIN32
#include "console/WindowsConsole.h"
#endif

int main(int argc, char* argv[])
{
#if defined Q_OS_WIN32
    // attach the parent console
    console::WindowsConsoleGuard _consoleGuard;
#endif

    FileLinkApp ldh(argc, argv);

    switch (ldh.status()) {
        case FileLinkApp::Starting:
        case FileLinkApp::Initialized: {
            return ldh.exec();
        }
        case FileLinkApp::Failed:
            return 1;
        case FileLinkApp::Succeeded:
            return 0;
        default:
            return -1;
    }
}
