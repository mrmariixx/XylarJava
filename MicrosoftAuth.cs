using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using CmlLib.Core.Auth;
using CmlLib.Core.Auth.Microsoft;

namespace XylarJavaLauncher;

internal static class MicrosoftAuth
{
    private static readonly string AccountStorePath = Path.Combine(
        AppContext.BaseDirectory,
        "microsoft_accounts.json");

    public static async Task<MicrosoftSignInLaunchResult> BeginSignInAsync(CancellationToken cancellationToken = default)
    {
        try
        {
            var handler = CreateLoginHandler();
            var session = await handler.AuthenticateInteractively(cancellationToken);
            return Success(session, "Microsoft account connected.");
        }
        catch (OperationCanceledException)
        {
            return Failure("Microsoft sign-in was canceled.");
        }
        catch (Exception ex)
        {
            return Failure(BuildFriendlyError(ex));
        }
    }

    public static async Task<MicrosoftSignInLaunchResult> RestoreSessionAsync(CancellationToken cancellationToken = default)
    {
        try
        {
            var handler = CreateLoginHandler();
            var session = await handler.AuthenticateSilently(cancellationToken);
            return Success(session, "Microsoft session restored.");
        }
        catch (OperationCanceledException)
        {
            return Failure("Microsoft sign-in was canceled.");
        }
        catch (Exception ex)
        {
            return Failure(BuildFriendlyError(ex));
        }
    }

    public static async Task SignOutAsync(CancellationToken cancellationToken = default)
    {
        try
        {
            var handler = CreateLoginHandler();
            await handler.Signout(cancellationToken);
        }
        catch
        {
            // Keep the launcher usable even if Microsoft sign-out fails.
        }
    }

    private static JELoginHandler CreateLoginHandler()
    {
        Directory.CreateDirectory(Path.GetDirectoryName(AccountStorePath) ?? AppContext.BaseDirectory);
        return new JELoginHandlerBuilder()
            .WithAccountManager(AccountStorePath)
            .Build();
    }

    private static MicrosoftSignInLaunchResult Success(MSession session, string message)
    {
        return new MicrosoftSignInLaunchResult
        {
            Success = true,
            Session = session,
            DisplayName = session.Username,
            Message = message
        };
    }

    private static MicrosoftSignInLaunchResult Failure(string message)
    {
        return new MicrosoftSignInLaunchResult
        {
            Success = false,
            ErrorMessage = message
        };
    }

    private static string BuildFriendlyError(Exception ex)
    {
        var message = ex.Message?.Trim();
        if (string.IsNullOrWhiteSpace(message))
            return "Microsoft sign-in failed.";

        if (message.Contains("user cancelled", StringComparison.OrdinalIgnoreCase) ||
            message.Contains("canceled", StringComparison.OrdinalIgnoreCase))
        {
            return "Microsoft sign-in was canceled.";
        }

        if (message.Contains("No account", StringComparison.OrdinalIgnoreCase) ||
            message.Contains("not signed in", StringComparison.OrdinalIgnoreCase))
        {
            return "No saved Microsoft session was found. Sign in again.";
        }

        return message;
    }
}

internal sealed class MicrosoftSignInLaunchResult
{
    public bool Success { get; init; }
    public string? DisplayName { get; init; }
    public string? Message { get; init; }
    public string? ErrorMessage { get; init; }
    public MSession? Session { get; init; }
}
