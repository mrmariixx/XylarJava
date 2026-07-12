#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <system_error>
namespace console {
void BindCrtHandlesToStdHandles(bool bindStdIn, bool bindStdOut, bool bindStdErr);
bool AttachWindowsConsole();
std::error_code EnableAnsiSupport();
void FreeWindowsConsole();

class WindowsConsoleGuard {
   public:
    WindowsConsoleGuard();
    ~WindowsConsoleGuard();

   private:
    bool m_consoleAttached;
};

}  // namespace console
