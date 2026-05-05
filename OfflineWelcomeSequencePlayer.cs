using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;

#if WINDOWS
using WinFormsApplication = System.Windows.Forms.Application;
using WinFormsForm = System.Windows.Forms.Form;
using WinFormsFormBorderStyle = System.Windows.Forms.FormBorderStyle;
using WinFormsFormStartPosition = System.Windows.Forms.FormStartPosition;
using WinFormsFormWindowState = System.Windows.Forms.FormWindowState;
using WinFormsKeys = System.Windows.Forms.Keys;
using WinFormsTimer = System.Windows.Forms.Timer;
using WinFormsWebBrowser = System.Windows.Forms.WebBrowser;
using WinFormsWebBrowserDocumentCompletedEventArgs = System.Windows.Forms.WebBrowserDocumentCompletedEventArgs;
using WinFormsWebBrowserNavigatingEventArgs = System.Windows.Forms.WebBrowserNavigatingEventArgs;
#endif

namespace XylarJavaLauncher;

public static class OfflineWelcomeSequencePlayer
{
    public static Task PlayAsync(string htmlPath)
    {
#if WINDOWS
        if (string.IsNullOrWhiteSpace(htmlPath) || !File.Exists(htmlPath))
            return Task.CompletedTask;

        return OfflineWelcomeSequenceForm.RunAsync(
            htmlPath,
            TimeSpan.FromSeconds(13.8),
            TimeSpan.FromSeconds(1.2));
#else
        return Task.CompletedTask;
#endif
    }

    public static Task<string?> CaptureOfflineUsernameAsync(string htmlPath)
    {
#if WINDOWS
        if (string.IsNullOrWhiteSpace(htmlPath) || !File.Exists(htmlPath))
            return Task.FromResult<string?>(null);

        return OfflineWelcomeSequenceForm.RunForUsernameAsync(
            htmlPath,
            TimeSpan.FromSeconds(5),
            TimeSpan.FromSeconds(1.2));
#else
        return Task.FromResult<string?>(null);
#endif
    }
}

#if WINDOWS
internal enum OfflineWelcomeMode
{
    SequenceOnly,
    CaptureUsername
}

internal sealed class OfflineWelcomeSequenceForm : WinFormsForm
{
    private readonly string _htmlPath;
    private readonly OfflineWelcomeMode _mode;
    private readonly TimeSpan _displayDuration;
    private readonly int _fadeSteps = 12;
    private readonly WinFormsWebBrowser _browser;
    private readonly WinFormsTimer _closeTimer;
    private readonly WinFormsTimer _fadeTimer;
    private bool _sequenceScheduled;
    private int _currentFadeStep;
    private string? _capturedUsername;

    private OfflineWelcomeSequenceForm(string htmlPath, OfflineWelcomeMode mode, TimeSpan displayDuration, TimeSpan fadeDuration)
    {
        _htmlPath = htmlPath;
        _mode = mode;
        _displayDuration = displayDuration;

        FormBorderStyle = WinFormsFormBorderStyle.None;
        StartPosition = WinFormsFormStartPosition.CenterScreen;
        WindowState = WinFormsFormWindowState.Maximized;
        BackColor = System.Drawing.Color.Black;
        ShowInTaskbar = false;
        TopMost = true;
        Opacity = 1d;
        KeyPreview = true;

        _browser = new WinFormsWebBrowser
        {
            Dock = System.Windows.Forms.DockStyle.Fill,
            ScriptErrorsSuppressed = true,
            IsWebBrowserContextMenuEnabled = false,
            WebBrowserShortcutsEnabled = false,
            AllowWebBrowserDrop = false,
            ScrollBarsEnabled = false
        };
        _browser.DocumentCompleted += Browser_DocumentCompleted;
        _browser.Navigating += Browser_Navigating;
        Controls.Add(_browser);

        _closeTimer = new WinFormsTimer
        {
            Interval = Math.Max(1000, (int)Math.Round(_displayDuration.TotalMilliseconds))
        };
        _closeTimer.Tick += (_, _) =>
        {
            _closeTimer.Stop();
            BeginFadeOut();
        };

        _fadeTimer = new WinFormsTimer
        {
            Interval = Math.Max(30, (int)Math.Round(fadeDuration.TotalMilliseconds / _fadeSteps))
        };
        _fadeTimer.Tick += (_, _) =>
        {
            _currentFadeStep++;
            Opacity = Math.Max(0d, 1d - ((double)_currentFadeStep / _fadeSteps));

            if (_currentFadeStep >= _fadeSteps)
            {
                _fadeTimer.Stop();
                Close();
            }
        };

        Shown += (_, _) => LoadOfflineHtml();
        KeyDown += (_, e) =>
        {
            if (e.KeyCode == WinFormsKeys.Escape)
                BeginFadeOut();
        };
    }

    public static Task RunAsync(string htmlPath, TimeSpan displayDuration, TimeSpan fadeDuration)
    {
        var tcs = new TaskCompletionSource<bool>(TaskCreationOptions.RunContinuationsAsynchronously);

        var thread = new Thread(() =>
        {
            try
            {
                WinFormsApplication.EnableVisualStyles();
                WinFormsApplication.SetCompatibleTextRenderingDefault(false);

                using var form = new OfflineWelcomeSequenceForm(htmlPath, OfflineWelcomeMode.SequenceOnly, displayDuration, fadeDuration);
                WinFormsApplication.Run(form);
                tcs.TrySetResult(true);
            }
            catch (Exception ex)
            {
                tcs.TrySetException(ex);
            }
        });

        thread.SetApartmentState(ApartmentState.STA);
        thread.IsBackground = true;
        thread.Start();

        return tcs.Task;
    }

    public static Task<string?> RunForUsernameAsync(string htmlPath, TimeSpan displayDuration, TimeSpan fadeDuration)
    {
        var tcs = new TaskCompletionSource<string?>(TaskCreationOptions.RunContinuationsAsynchronously);

        var thread = new Thread(() =>
        {
            try
            {
                WinFormsApplication.EnableVisualStyles();
                WinFormsApplication.SetCompatibleTextRenderingDefault(false);

                using var form = new OfflineWelcomeSequenceForm(htmlPath, OfflineWelcomeMode.CaptureUsername, displayDuration, fadeDuration);
                WinFormsApplication.Run(form);
                tcs.TrySetResult(form._capturedUsername);
            }
            catch (Exception ex)
            {
                tcs.TrySetException(ex);
            }
        });

        thread.SetApartmentState(ApartmentState.STA);
        thread.IsBackground = true;
        thread.Start();

        return tcs.Task;
    }

    private void LoadOfflineHtml()
    {
        try
        {
            var html = File.ReadAllText(_htmlPath);
            var injection = _mode == OfflineWelcomeMode.CaptureUsername
                ? "<script>window.__XYLAR_EMBEDDED_WELCOME__ = true; window.__XYLAR_CLOSE_AFTER_MS__ = 5000;</script>"
                : "<script>window.__XYLAR_OFFLINE_SEQUENCE__ = true;</script>";
            html = html.Contains("</head>", StringComparison.OrdinalIgnoreCase)
                ? html.Replace("</head>", injection + Environment.NewLine + "</head>", StringComparison.OrdinalIgnoreCase)
                : injection + Environment.NewLine + html;

            _browser.DocumentText = html;
        }
        catch
        {
            ScheduleCloseOnce();
        }
    }

    private void Browser_DocumentCompleted(object? sender, WinFormsWebBrowserDocumentCompletedEventArgs e)
    {
        if (_mode == OfflineWelcomeMode.CaptureUsername)
            return;

        ScheduleCloseOnce();
    }

    private void Browser_Navigating(object? sender, WinFormsWebBrowserNavigatingEventArgs e)
    {
        if (_mode != OfflineWelcomeMode.CaptureUsername || e.Url == null)
            return;

        if (!string.Equals(e.Url.Scheme, "xylar", StringComparison.OrdinalIgnoreCase))
            return;

        e.Cancel = true;
        _capturedUsername = ParseSignalUsername(e.Url);
        BeginFadeOut();
    }

    private void ScheduleCloseOnce()
    {
        if (_sequenceScheduled)
            return;

        _sequenceScheduled = true;
        _closeTimer.Start();
    }

    private void BeginFadeOut()
    {
        if (_fadeTimer.Enabled)
            return;

        _currentFadeStep = 0;
        _fadeTimer.Start();
    }

    private static string? ParseSignalUsername(Uri uri)
    {
        var query = uri.Query;
        if (string.IsNullOrWhiteSpace(query))
            return null;

        if (query.StartsWith("?", StringComparison.Ordinal))
            query = query[1..];

        foreach (var part in query.Split('&', StringSplitOptions.RemoveEmptyEntries))
        {
            var pieces = part.Split('=', 2, StringSplitOptions.None);
            if (pieces.Length != 2)
                continue;

            if (!string.Equals(pieces[0], "username", StringComparison.OrdinalIgnoreCase))
                continue;

            var value = Uri.UnescapeDataString(pieces[1]);
            return string.IsNullOrWhiteSpace(value) ? null : value.Trim();
        }

        return null;
    }

    protected override void Dispose(bool disposing)
    {
        if (disposing)
        {
            _closeTimer.Dispose();
            _fadeTimer.Dispose();
            _browser.Dispose();
        }

        base.Dispose(disposing);
    }
}
#endif
