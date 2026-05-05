using Avalonia;
using System;
#if WINDOWS
using WinFormsApplication = System.Windows.Forms.Application;
using WinFormsHighDpiMode = System.Windows.Forms.HighDpiMode;
#endif

namespace XylarJavaLauncher;

class Program
{
    [STAThread]
    public static void Main(string[] args)
    {
#if WINDOWS
        WinFormsApplication.SetHighDpiMode(WinFormsHighDpiMode.PerMonitorV2);
        WinFormsApplication.EnableVisualStyles();
#endif
        BuildAvaloniaApp().StartWithClassicDesktopLifetime(args);
    }

    public static AppBuilder BuildAvaloniaApp()
        => AppBuilder.Configure<App>()
            .UsePlatformDetect()
            .LogToTrace();
}
