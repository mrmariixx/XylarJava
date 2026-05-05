using System;
using System.Diagnostics;
using System.IO;
using System.Threading.Tasks;
using Avalonia.Controls;
using Avalonia.Interactivity;

namespace XylarJavaLauncher;

public partial class WelcomeWindow : Window
{
    private const string DefaultOfflineUsername = "OfflineUser";

    public WelcomeWindow()
    {
        InitializeComponent();
    }

    protected override void OnOpened(EventArgs e)
    {
        base.OnOpened(e);
        _ = RunIntroSequenceAsync();
    }

    private async Task RunIntroSequenceAsync()
    {
        await Task.Delay(120);
        await FadeAsync(StagePanel, 0, 1, 260);
    }

    private static async Task FadeAsync(Control? control, double from, double to, int durationMs)
    {
        if (control == null)
            return;

        const int steps = 18;
        control.Opacity = from;

        for (int i = 1; i <= steps; i++)
        {
            control.Opacity = from + ((to - from) * i / steps);
            await Task.Delay(Math.Max(1, durationMs / steps));
        }
    }

    private async void MicrosoftSignInButton_Click(object? sender, RoutedEventArgs e)
    {
        SetButtonsEnabled(false);
        var launch = await MicrosoftAuth.BeginSignInAsync();
        if (!launch.Success)
        {
            SetButtonsEnabled(true);
            Debug.WriteLine($"Could not open browser: {launch.ErrorMessage}");
            return;
        }

        Close(new WelcomeWindowResult
        {
            Username = string.IsNullOrWhiteSpace(launch.DisplayName) ? DefaultOfflineUsername : launch.DisplayName,
            AccountMode = "Microsoft",
            OpenedMicrosoftSignIn = true,
            MicrosoftStatusMessage = launch.Message,
            MicrosoftDisplayName = launch.DisplayName,
            PlayOfflineSequence = false
        });
    }

    private async void GoOfflineButton_Click(object? sender, RoutedEventArgs e)
    {
        SetButtonsEnabled(false);

        string username = DefaultOfflineUsername;

        try
        {
            var htmlPath = Path.Combine(AppContext.BaseDirectory, "WelcomePage.html");
            var htmlUsername = await OfflineWelcomeSequencePlayer.CaptureOfflineUsernameAsync(htmlPath);
            if (!string.IsNullOrWhiteSpace(htmlUsername))
                username = htmlUsername.Trim();
        }
        catch (Exception ex)
        {
            Debug.WriteLine($"Offline welcome fallback: {ex.Message}");
        }

        Close(new WelcomeWindowResult
        {
            Username = username,
            AccountMode = "Offline",
            OpenedMicrosoftSignIn = false,
            PlayOfflineSequence = false
        });
    }

    private void SetButtonsEnabled(bool enabled)
    {
        if (MicrosoftSignInButton != null)
            MicrosoftSignInButton.IsEnabled = enabled;

        if (GoOfflineButton != null)
            GoOfflineButton.IsEnabled = enabled;
    }
}

public class WelcomeWindowResult
{
    public string Username { get; set; } = "OfflineUser";
    public string AccountMode { get; set; } = "Offline";
    public bool OpenedMicrosoftSignIn { get; set; }
    public string? MicrosoftStatusMessage { get; set; }
    public string? MicrosoftDisplayName { get; set; }
    public bool PlayOfflineSequence { get; set; }
}
