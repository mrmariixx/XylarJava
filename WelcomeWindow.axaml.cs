using System;
using System.Diagnostics;
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
            MicrosoftDisplayName = launch.DisplayName
        });
    }

    private async void GoOfflineButton_Click(object? sender, RoutedEventArgs e)
    {
        if (IntroPanel == null || OfflineNamePanel == null || OfflineUsernameBox == null)
            return;

        SetButtonsEnabled(false);
        await FadeAsync(IntroPanel, 1, 0, 180);
        IntroPanel.IsVisible = false;
        OfflineNamePanel.IsVisible = true;
        await FadeAsync(OfflineNamePanel, 0, 1, 220);
        OfflineUsernameBox.Text = DefaultOfflineUsername;
        OfflineUsernameBox.Focus();
        SetOfflineNameButtonsEnabled(true);
    }

    private void ContinueOfflineButton_Click(object? sender, RoutedEventArgs e)
    {
        var username = string.IsNullOrWhiteSpace(OfflineUsernameBox?.Text)
            ? DefaultOfflineUsername
            : OfflineUsernameBox!.Text!.Trim();

        Close(new WelcomeWindowResult
        {
            Username = username,
            AccountMode = "Offline",
            OpenedMicrosoftSignIn = false
        });
    }

    private async void BackToChoiceButton_Click(object? sender, RoutedEventArgs e)
    {
        if (IntroPanel == null || OfflineNamePanel == null)
            return;

        SetOfflineNameButtonsEnabled(false);
        await FadeAsync(OfflineNamePanel, 1, 0, 160);
        OfflineNamePanel.IsVisible = false;
        IntroPanel.IsVisible = true;
        await FadeAsync(IntroPanel, 0, 1, 200);
        SetButtonsEnabled(true);
    }

    private void SetButtonsEnabled(bool enabled)
    {
        if (MicrosoftSignInButton != null)
            MicrosoftSignInButton.IsEnabled = enabled;

        if (GoOfflineButton != null)
            GoOfflineButton.IsEnabled = enabled;
    }

    private void SetOfflineNameButtonsEnabled(bool enabled)
    {
        if (ContinueOfflineButton != null)
            ContinueOfflineButton.IsEnabled = enabled;

        if (BackToChoiceButton != null)
            BackToChoiceButton.IsEnabled = enabled;

        if (OfflineUsernameBox != null)
            OfflineUsernameBox.IsEnabled = enabled;
    }
}

public class WelcomeWindowResult
{
    public string Username { get; set; } = "OfflineUser";
    public string AccountMode { get; set; } = "Offline";
    public bool OpenedMicrosoftSignIn { get; set; }
    public string? MicrosoftStatusMessage { get; set; }
    public string? MicrosoftDisplayName { get; set; }
}
