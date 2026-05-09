using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Reflection;
using System.Text.Json;
using System.Threading;
using System.Threading.Tasks;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Controls.Primitives;
using Avalonia.Input;
using Avalonia.Interactivity;
using Avalonia.Media;
using Avalonia.Media.Imaging;
using Avalonia.Platform;
using Avalonia.Threading;
using CmlLib.Core;
using CmlLib.Core.Auth;
using CmlLib.Core.Installer.Forge;
using CmlLib.Core.ProcessBuilder;

namespace XylarJavaLauncher;

public partial class MainWindow : Window
{
    private const string DefaultOfflineUsername = "OfflineUser";
    private const string AccountModeOffline = "Offline";
    private const string AccountModeMicrosoft = "Microsoft";
    private readonly MinecraftLauncher _launcher = null!;
    private readonly MinecraftPath _minecraftPath = null!;
    private readonly string _skinsFolder = string.Empty;
    private readonly string _skinSettingsFile = string.Empty;
    private readonly string _instancesFolder = string.Empty;
    private readonly string _profileSettingsFile = string.Empty;
    private readonly HttpClient _httpClient = new();
    private CancellationTokenSource? _modpackLoadCts;
    private string _versionFilter = "Release";
    private List<ModpackItem> _modpacks = new();
    private readonly ObservableCollection<ModpackItem> _displayedModpacks = new();
    private readonly ContentService? _contentService;
    private readonly ObservableCollection<ContentItem> _contentItems = new();
    private ContentFilter _contentFilter = new()
    {
        ContentType = ContentType.Modpack,
        SelectedSources = new() { ContentSource.Modrinth }
    };
    private CancellationTokenSource? _contentLoadCts;
    private CancellationTokenSource? _contentThumbCts;
    private CancellationTokenSource? _contentSearchDebounceCts;
    private readonly Dictionary<string, Bitmap> _contentThumbnailCache = new(StringComparer.OrdinalIgnoreCase);
    private readonly SemaphoreSlim _thumbnailConcurrency = new(4, 4);
    private int _contentPage = 1;
    private const int ContentPageSize = 60;
    private bool _contentHasMore;
    private readonly ObservableCollection<PlayProfile> _playProfiles = new();
    private PlayProfile _activePlayProfile = PlayProfile.Standard;
    private bool _suppressProfileReaction;
    private bool _isInitialized = false;
    private TaskCompletionSource<WelcomeWindowResult?>? _inlineWelcomeTcs;
    private bool _welcomeEditMode;
    private ContentItem? _currentDetailsItem;
    private List<ModrinthAPI.ModVersion> _currentInstallVersions = new();
    private string _currentInstallTargetPath = string.Empty;
    private string _currentInstallTargetLabel = "Standard profile (.minecraft)";
    private bool _isRefreshingInstallSheet;
    private LauncherProfile _launcherProfile = new();

public MainWindow()
{
    InitializeComponent();

    _contentService = new ContentService(_httpClient);

    try
    {
        _minecraftPath = new MinecraftPath();
        _launcher = new MinecraftLauncher(_minecraftPath);

        _skinsFolder = Path.Combine(AppContext.BaseDirectory, "skins");
        _skinSettingsFile = Path.Combine(_skinsFolder, "active_skin.json");
        _instancesFolder = Path.Combine(AppContext.BaseDirectory, "instances");
        _profileSettingsFile = Path.Combine(AppContext.BaseDirectory, "launcher_profile.json");

        Directory.CreateDirectory(_skinsFolder);
        Directory.CreateDirectory(_instancesFolder);
        _httpClient.DefaultRequestHeaders.UserAgent.ParseAdd("XylarLauncher/0.2 (+https://modrinth.com)");

        UsernameBox.Text ??= DefaultOfflineUsername;

        if (ProfileTargetCombo != null)
            ProfileTargetCombo.ItemsSource = _playProfiles;

        _ = LoadVersionsAsync();
        // Defer heavy catalog sync so Home opens faster.

        NavMain.IsChecked = true;
        MainSection.IsVisible = true;
        SkinSection.IsVisible = false;
        OptionsSection.IsVisible = false;
        ModSection.IsVisible = false;
        CreditsSection.IsVisible = false;
        SetHeader("Home", string.Empty);

        _isInitialized = true;
        SetHeader("Home", string.Empty);
        
        // Register event handlers AFTER initialization
        LoaderCombo.SelectionChanged += LoaderCombo_SelectionChanged;
        VersionCombo.SelectionChanged += VersionCombo_SelectionChanged;
        if (ProfileTargetCombo != null)
            ProfileTargetCombo.SelectionChanged += ProfileTargetCombo_SelectionChanged;
        UsernameBox.TextChanged += UsernameBox_TextChanged;

        ApplyExtremeLiquidGlassEffect();
        
        RefreshPlayProfiles();
        UpdateContentBrowserCopy();

        Opened += async (_, _) =>
        {
            LoadEmbeddedBrandAssets();
            await RunWelcomeIfNeededAsync();
        };

        UpdateStatus(CheckJava21Oracle()
            ? "Ready to play."
            : "Ready to play. Java will be checked only when you launch Minecraft.");
    }
    catch (Exception ex)
    {
        UpdateStatus($"Initialization failed: {ex.Message}");
    }
}

    private void ApplyExtremeLiquidGlassEffect()
    {
        if (NavGlassHost != null)
            LiquidGlassEffects.ApplyExtremeSidebarLiquidGlass(NavGlassHost);
    }

    private void UpdateStatus(string message)
    {
        if (StatusText != null) StatusText.Text = message;
        if (HeroStatusText != null) HeroStatusText.Text = message;
    }

    private void UpdateSelectionSummary()
    {
        if (!_isInitialized || VersionCombo == null || LoaderCombo == null || UsernameBox == null) return;

        var player = string.IsNullOrWhiteSpace(UsernameBox.Text) ? DefaultOfflineUsername : UsernameBox.Text.Trim();
        var version = VersionCombo.SelectedItem?.ToString();
        var versionText = string.IsNullOrWhiteSpace(version) ? "not selected" : version;
        var loader = GetSelectedLoader();
        _launcherProfile.Username = player;

        if (HeroVersionText != null) HeroVersionText.Text = $"Version: {versionText}";
        if (HeroLoaderText != null) HeroLoaderText.Text = $"Loader: {loader}";
        if (CurrentVersionLabel != null) CurrentVersionLabel.Text = $"Version: {versionText}";
        if (CurrentLoaderLabel != null) CurrentLoaderLabel.Text = $"Loader: {loader}";
        if (CurrentPlayerLabel != null) CurrentPlayerLabel.Text = $"Launch name: {player}";
        if (SettingsLaunchNameText != null) SettingsLaunchNameText.Text = $"Launch name: {player}";
        if (SelectionSummaryText != null) SelectionSummaryText.Text = $"{loader} profile - {versionText} - {player}";
        if (SelectionSummaryLabel != null)
            SelectionSummaryLabel.Text = string.Equals(_launcherProfile.AccountMode, AccountModeMicrosoft, StringComparison.OrdinalIgnoreCase)
                ? $"Account mode: Microsoft ({GetMicrosoftAccountLabel()})."
                : "Account mode: Offline-ready local profile.";
    }

    private string GetMicrosoftAccountLabel()
    {
        return string.IsNullOrWhiteSpace(_launcherProfile.MicrosoftDisplayName)
            ? "not connected"
            : _launcherProfile.MicrosoftDisplayName!;
    }

    private string GetSelectedLoader()
    {
        if (!_isInitialized || LoaderCombo == null) return "Fabric";
        return (LoaderCombo.SelectedItem as ComboBoxItem)?.Content?.ToString() ?? "Fabric";
    }

    private void UpdateLoaderMessage()
    {
        if (!_isInitialized || LoaderWarning == null) return;
        var loader = GetSelectedLoader();
        LoaderWarning.Text = loader switch
        {
            "Forge" => "Forge profile for the selected version.",
            "Vanilla" => "Base Minecraft without a loader.",
            _ => "Fabric profile for the selected version."
        };
        UpdateSelectionSummary();
    }

    private async Task LoadVersionsAsync()
    {
        try
        {
            if (StatusText == null) return;

            UpdateStatus("Loading versions...");
            var versions = await _launcher.GetAllVersionsAsync();
            if (versions == null || !versions.Any())
            {
                UpdateStatus("No versions were found yet.");
                return;
            }

            var filtered = versions.Where(v => _versionFilter switch
            {
                "Release"   => v.Type == "release",
                "Installed" => Directory.Exists(Path.Combine(_minecraftPath.BasePath, "versions", v.Name)),
                _           => true,
            }).Select(v => v.Name).ToList();

            if (VersionCombo != null)
            {
                VersionCombo.ItemsSource = filtered;
                if (filtered.Any())
                    VersionCombo.SelectedItem = filtered[0];
            }

            UpdateSelectionSummary();
            UpdateStatus(filtered.Any() ? $"Loaded {filtered.Count} versions." : "No versions matched this filter.");
        }
        catch (Exception ex)
        {
            if (StatusText != null)
                UpdateStatus($"Version loading failed: {ex.Message}");
        }
    }

    private void VersionFilterButton_Click(object? sender, RoutedEventArgs e)
    {
        ReleaseFilter.IsChecked = sender == ReleaseFilter;
        InstalledFilter.IsChecked = sender == InstalledFilter;
        AllFilter.IsChecked = sender == AllFilter;

        _versionFilter = ReleaseFilter.IsChecked == true ? "Release"
            : InstalledFilter.IsChecked == true ? "Installed"
            : "All";

        _ = LoadVersionsAsync();
    }

    private void RefreshVersionsButton_Click(object? sender, RoutedEventArgs e)
    {
        _ = LoadVersionsAsync();
    }

    private async void LaunchButton_Click(object? sender, RoutedEventArgs e)
    {
        try
        {
            var username = UsernameBox.Text?.Trim();
            if (string.IsNullOrWhiteSpace(username))
                username = DefaultOfflineUsername;

            var mcVersion = VersionCombo.SelectedItem?.ToString();
            if (string.IsNullOrWhiteSpace(mcVersion))
            {
                UpdateStatus("Choose a Minecraft version first.");
                return;
            }

            var loader = GetSelectedLoader();
            string? gameDir = null;
            string? pinnedLoaderVersion = null;

            if (_activePlayProfile is { IsStandard: false } pack)
            {
                if (!string.IsNullOrWhiteSpace(pack.MinecraftVersion))
                    mcVersion = pack.MinecraftVersion;
                loader = pack.Loader;
                pinnedLoaderVersion = pack.LoaderVersion;
                gameDir = pack.GameDirectory;
            }

            UpdateSelectionSummary();
            UpdateStatus($"Preparing {loader}…");
            DownloadProgress.Value = 0;

            var launchAttempt = 0;
            const int maxAttempts = 3;
            Exception? lastException = null;

            while (launchAttempt < maxAttempts)
            {
                try
                {
                    launchAttempt++;
                    if (launchAttempt > 1)
                        UpdateStatus($"Attempt {launchAttempt}/{maxAttempts}: Auto-recovery…");

                    var launchVersionId = mcVersion;
                    var forgeOpts = new ForgeInstallOptions { SkipIfAlreadyInstalled = true };
                    if (!string.Equals(loader, "Vanilla", StringComparison.OrdinalIgnoreCase))
                    {
                        if (string.Equals(loader, "Forge", StringComparison.OrdinalIgnoreCase))
                        {
                            UpdateStatus($"Installing Forge {mcVersion}…");
                            var forgeInst = new ForgeInstaller(_launcher, _httpClient);
                            try
                            {
                                launchVersionId = string.IsNullOrWhiteSpace(pinnedLoaderVersion)
                                    ? await forgeInst.Install(mcVersion, forgeOpts)
                                    : await forgeInst.Install(mcVersion, pinnedLoaderVersion, forgeOpts);
                                if (LoaderWarning != null) LoaderWarning.Text = $"Forge {mcVersion} ready.";
                            }
                            catch (Exception forgeEx)
                            {
                                if (launchAttempt < maxAttempts)
                                {
                                    await CleanupForgeArtifacts(mcVersion);
                                    UpdateStatus($"Forge error detected; cleaning cache and retrying…");
                                    await Task.Delay(1000);
                                    throw new Exception($"Forge install requires retry: {forgeEx.Message}", forgeEx);
                                }
                                throw;
                            }
                        }
                        else if (string.Equals(loader, "Fabric", StringComparison.OrdinalIgnoreCase))
                        {
                            UpdateStatus($"Installing {loader} {mcVersion}…");
                            var ok = await RunFabricInstallerAsync(mcVersion, loader, pinnedLoaderVersion, launchAttempt, maxAttempts);
                            if (!ok)
                            {
                                if (launchAttempt < maxAttempts)
                                {
                                    await CleanupFabricArtifacts(mcVersion, loader);
                                    UpdateStatus($"{loader} setup failed; cleaning artifacts and retrying…");
                                    await Task.Delay(1500);
                                    throw new Exception($"{loader} install requires retry.");
                                }
                                UpdateStatus($"{loader} install failed after {maxAttempts} attempts. Check Java PATH and system resources.");
                                return;
                            }

                            launchVersionId = DiscoverFabricVersionId(loader, mcVersion);
                            if (string.IsNullOrWhiteSpace(launchVersionId))
                            {
                                if (launchAttempt < maxAttempts)
                                {
                                    UpdateStatus($"{loader} profile not found; retrying discovery…");
                                    await Task.Delay(500);
                                    throw new Exception($"{loader} profile discovery requires retry.");
                                }
                                UpdateStatus($"{loader} installed but profile not found. Restart launcher and try again.");
                                return;
                            }

                            if (LoaderWarning != null) LoaderWarning.Text = $"{loader} profile: {launchVersionId}";
                        }
                        else
                        {
                            UpdateStatus($"Loader \"{loader}\" is not supported.");
                            return;
                        }
                    }

                    UpdateStatus("Verifying game files…");
                    await _launcher.InstallAsync(launchVersionId);

                    var option = await CreateLaunchOptionAsync(username, gameDir);
                    UpdateStatus("Launching game…");
                    var process = await _launcher.BuildProcessAsync(launchVersionId, option);
                    process.Start();

                    UpdateStatus("Game launched successfully.");
                    DownloadProgress.Value = 100;
                    return;
                }
                catch (Exception ex)
                {
                    lastException = ex;
                    if (launchAttempt < maxAttempts)
                    {
                        var msg = ex.Message.ToLower();
                        if (msg.Contains("java") || msg.Contains("jvm"))
                        {
                            UpdateStatus($"Java issue detected; verifying environment (attempt {launchAttempt}/{maxAttempts})…");
                            await ValidateJavaEnvironment();
                        }
                        else if (msg.Contains("version") || msg.Contains("manifest"))
                        {
                            UpdateStatus($"Version issue detected; clearing cache (attempt {launchAttempt}/{maxAttempts})…");
                            await ClearVersionCache(mcVersion);
                        }
                        else if (msg.Contains("network") || msg.Contains("http"))
                        {
                            UpdateStatus($"Network issue detected; waiting before retry (attempt {launchAttempt}/{maxAttempts})…");
                            await Task.Delay(3000);
                        }
                        else
                        {
                            UpdateStatus($"Launch issue detected; recovering (attempt {launchAttempt}/{maxAttempts})…");
                            await CleanupLaunchEnvironment();
                        }
                        await Task.Delay(1000);
                    }
                    else
                    {
                        break;
                    }
                }
            }

            if (lastException != null)
            {
                var errorMsg = lastException.Message;
                if (errorMsg.Length > 120)
                    errorMsg = errorMsg.Substring(0, 120) + "…";
                UpdateStatus($"Launch failed after {maxAttempts} attempts: {errorMsg}");
            }
        }
        catch (Exception ex)
        {
            UpdateStatus($"Fatal error: {ex.Message}");
        }
    }

    private async Task<MLaunchOption> CreateLaunchOptionAsync(string username, string? gameDirectory)
    {
        var session = await ResolveLaunchSessionAsync(username);
        var option = new MLaunchOption { Session = session };
        if (!string.IsNullOrWhiteSpace(gameDirectory))
        {
            option.ExtraGameArguments = new[]
            {
                new MArgument("--gameDir"),
                new MArgument(gameDirectory!)
            };
        }

        // OfflineSkinLaunchHelper.ApplySkinToLaunchOptions(
        //     _minecraftPath,
        //     username,
        //     _skinSettingsFile,
        //     _skinsFolder,
        //     option);
        return option;
    }

    private async Task<MSession> ResolveLaunchSessionAsync(string offlineUsername)
    {
        if (!string.Equals(_launcherProfile.AccountMode, AccountModeMicrosoft, StringComparison.OrdinalIgnoreCase))
            return MSession.CreateOfflineSession(offlineUsername);

        UpdateStatus("Refreshing Microsoft session...");

        var restored = await MicrosoftAuth.RestoreSessionAsync();
        if (restored.Success && restored.Session != null)
        {
            PersistMicrosoftIdentity(restored);
            return restored.Session;
        }

        UpdateStatus("Microsoft session expired. Sign in again...");
        var interactive = await MicrosoftAuth.BeginSignInAsync();
        if (interactive.Success && interactive.Session != null)
        {
            PersistMicrosoftIdentity(interactive);
            return interactive.Session;
        }

        throw new InvalidOperationException(interactive.ErrorMessage ?? restored.ErrorMessage ?? "Microsoft sign-in is required before launch.");
    }

    private string? DiscoverFabricVersionId(string loader, string mcVersion)
    {
        var versionsDir = Path.Combine(_minecraftPath.BasePath, "versions");
        try
        {
            if (!Directory.Exists(versionsDir))
                return null;

            var prefix = "fabric-loader-";

            return Directory.GetDirectories(versionsDir)
                .Select(static d => Path.GetFileName(d))
                .Where(n => !string.IsNullOrEmpty(n))
                .Where(n => n.StartsWith(prefix, StringComparison.OrdinalIgnoreCase) &&
                            n.Contains(mcVersion, StringComparison.OrdinalIgnoreCase))
                .OrderByDescending(static n => n!.Length)
                .FirstOrDefault();
        }
        catch
        {
            return null;
        }
    }

    private async Task<bool> RunFabricInstallerAsync(string version, string loader, string? loaderVersion, int attempt = 1, int maxAttempts = 3)
    {
        try
        {
            var loaderKey = loader.ToLowerInvariant();
            var loaderUrl = loaderKey switch
            {
                "fabric" => await GetFabricInstallerUrlAsync(version),
                _        => null
            };

            if (string.IsNullOrWhiteSpace(loaderUrl))
            {
                UpdateStatus($"{loader} is not available for version {version}.");
                return false;
            }

            var installerPath = Path.Combine(AppContext.BaseDirectory, $"{loaderKey}-installer-{Guid.NewGuid():N}.jar");
            var maxRetries = 3;
            var downloadRetry = 0;

            while (downloadRetry < maxRetries)
            {
                try
                {
                    UpdateStatus($"Downloading {loader} installer…");
                    using var response = await _httpClient.GetAsync(loaderUrl, HttpCompletionOption.ResponseHeadersRead);
                    if (!response.IsSuccessStatusCode)
                    {
                        if (downloadRetry < maxRetries - 1)
                        {
                            downloadRetry++;
                            await Task.Delay(2000);
                            continue;
                        }
                        UpdateStatus($"{loader} download failed: HTTP {response.StatusCode}");
                        return false;
                    }

                    await using var contentStream = await response.Content.ReadAsStreamAsync();
                    await using var fileStream = File.Create(installerPath);
                    await contentStream.CopyToAsync(fileStream);
                    break;
                }
                catch when (downloadRetry < maxRetries - 1)
                {
                    downloadRetry++;
                    await Task.Delay(2000);
                }
                catch (Exception dlEx)
                {
                    UpdateStatus($"{loader} download error: {dlEx.Message}");
                    return false;
                }
            }

            if (!File.Exists(installerPath) || new FileInfo(installerPath).Length < 1024)
            {
                UpdateStatus($"{loader} installer download incomplete.");
                return false;
            }

            UpdateStatus($"Installing {loader} {version}…");

            var loaderArg = string.IsNullOrWhiteSpace(loaderVersion) ? "" : $" -loader {loaderVersion}";
            var args = $"-jar \"{installerPath}\" client -mcversion {version} -downloadMinecraft{loaderArg}";

            var processInfo = new ProcessStartInfo
            {
                FileName               = "java",
                Arguments              = args,
                UseShellExecute        = false,
                RedirectStandardOutput = true,
                RedirectStandardError  = true,
                CreateNoWindow         = false,
                WorkingDirectory       = _minecraftPath.BasePath
            };

            using var proc = Process.Start(processInfo);
            if (proc == null)
            {
                UpdateStatus($"Failed to start {loader} installer.");
                return false;
            }

            var completed = proc.WaitForExit(TimeSpan.FromMinutes(10));
            if (!completed)
            {
                try { proc.Kill(); } catch { }
                UpdateStatus($"{loader} installer timed out (>10 min).");
                return false;
            }

            var ok = proc.ExitCode == 0;
            if (!ok)
            {
                var errorOutput = proc.StandardError.ReadToEnd();
                if (errorOutput.Length > 150)
                    errorOutput = errorOutput.Substring(0, 150) + "…";
                UpdateStatus($"{loader} exit code {proc.ExitCode}: {errorOutput}");
            }
            else
            {
                UpdateStatus($"{loader} {version} installed.");
            }

            try
            {
                if (File.Exists(installerPath))
                    File.Delete(installerPath);
            }
            catch { }

            return ok;
        }
        catch (Exception ex)
        {
            var msg = ex.Message;
            if (msg.Length > 100)
                msg = msg.Substring(0, 100) + "…";
            UpdateStatus($"{loader} error: {msg}");
            return false;
        }
    }

    private async Task ValidateJavaEnvironment()
    {
        await Task.Run(() =>
        {
            try
            {
                var psi = new ProcessStartInfo
                {
                    FileName = "java",
                    Arguments = "-version",
                    UseShellExecute = false,
                    RedirectStandardError = true,
                    RedirectStandardOutput = true,
                    CreateNoWindow = true
                };
                using var proc = Process.Start(psi);
                if (proc != null && !proc.WaitForExit(5000))
                {
                    try { proc.Kill(); } catch { }
                }
            }
            catch { }
        });
    }

    private async Task ClearVersionCache(string version)
    {
        await Task.Run(() =>
        {
            try
            {
                var versionDir = Path.Combine(_minecraftPath.BasePath, "versions", version);
                if (Directory.Exists(versionDir))
                {
                    var jsonFile = Path.Combine(versionDir, $"{version}.json");
                    if (File.Exists(jsonFile))
                        File.Delete(jsonFile);
                }
            }
            catch { }
        });
    }

    private async Task CleanupForgeArtifacts(string version)
    {
        await Task.Run(() =>
        {
            try
            {
                var forgeVersions = Path.Combine(_minecraftPath.BasePath, "versions");
                if (Directory.Exists(forgeVersions))
                {
                    foreach (var dir in Directory.GetDirectories(forgeVersions))
                    {
                        var name = Path.GetFileName(dir);
                        if (name.Contains("forge", StringComparison.OrdinalIgnoreCase) && name.Contains(version))
                        {
                            try { Directory.Delete(dir, true); } catch { }
                        }
                    }
                }
            }
            catch { }
        });
    }

    private async Task CleanupFabricArtifacts(string version, string loader)
    {
        await Task.Run(() =>
        {
            try
            {
                var versionsDir = Path.Combine(_minecraftPath.BasePath, "versions");
                if (Directory.Exists(versionsDir))
                {
                    var prefix = "fabric-loader-";
                    foreach (var dir in Directory.GetDirectories(versionsDir))
                    {
                        var name = Path.GetFileName(dir);
                        if (name.StartsWith(prefix, StringComparison.OrdinalIgnoreCase) && name.Contains(version))
                        {
                            try { Directory.Delete(dir, true); } catch { }
                        }
                    }
                }
                var libDir = Path.Combine(_minecraftPath.BasePath, "libraries");
                if (Directory.Exists(libDir))
                {
                    try { Directory.Delete(libDir, true); } catch { }
                }
            }
            catch { }
        });
    }

    private async Task CleanupLaunchEnvironment()
    {
        await Task.Run(() =>
        {
            try
            {
                var nativesDir = Path.Combine(_minecraftPath.BasePath, "versions");
                if (Directory.Exists(nativesDir))
                {
                    foreach (var dir in Directory.GetDirectories(nativesDir))
                    {
                        try
                        {
                            var nativesSubdir = Path.Combine(dir, "natives");
                            if (Directory.Exists(nativesSubdir))
                                Directory.Delete(nativesSubdir, true);
                        }
                        catch { }
                    }
                }
            }
            catch { }
        });
    }

    private async Task<string?> GetFabricInstallerUrlAsync(string version)
    {
        try
        {
            using var response = await _httpClient.GetAsync("https://meta.fabricmc.net/v2/versions/installer");
            var json = await response.Content.ReadAsStringAsync();
            using var doc = JsonDocument.Parse(json);
            var latestVersion = doc.RootElement.EnumerateArray().FirstOrDefault().GetProperty("version").GetString();

            if (!string.IsNullOrWhiteSpace(latestVersion))
                return $"https://maven.fabricmc.net/net/fabricmc/fabric-installer/{latestVersion}/fabric-installer-{latestVersion}.jar";
        }
        catch { }
        return null;
    }

    private void RefreshPlayProfiles()
    {
        if (ProfileTargetCombo == null)
            return;

        var keepId = _activePlayProfile.Id;
        _playProfiles.Clear();
        _playProfiles.Add(PlayProfile.Standard);

        if (Directory.Exists(_instancesFolder))
        {
            foreach (var sub in Directory.GetDirectories(_instancesFolder))
            {
                var root = ModrinthModpackInstaller.FindPackRoot(sub);
                if (string.IsNullOrEmpty(root))
                    continue;

                var info = ModrinthPackIndexReader.TryRead(root);
                if (info == null)
                    continue;

                _playProfiles.Add(new PlayProfile
                {
                    Id                = root,
                    Title             = $"{info.Name} (installed)",
                    GameDirectory     = root,
                    MinecraftVersion  = info.MinecraftVersion,
                    Loader            = info.Loader,
                    LoaderVersion     = info.LoaderVersion
                });
            }
        }

        _suppressProfileReaction = true;
        var pick = _playProfiles.FirstOrDefault(p => p.Id == keepId) ?? PlayProfile.Standard;
        ProfileTargetCombo.SelectedItem = pick;
        _suppressProfileReaction = false;
        OnProfileSelectionApplied(pick);
    }

    private void ProfileTargetCombo_SelectionChanged(object? sender, SelectionChangedEventArgs e)
    {
        if (!_isInitialized || _suppressProfileReaction)
            return;
        var p = ProfileTargetCombo?.SelectedItem as PlayProfile ?? PlayProfile.Standard;
        OnProfileSelectionApplied(p);
    }

    private void OnProfileSelectionApplied(PlayProfile p)
    {
        _activePlayProfile = p;
        UpdateProfileUiLocks();
        UpdateActiveTargetFromProfile();
        UpdateSelectionSummary();
        if (!p.IsStandard)
            _ = ApplyModpackSelectionAsync(p);
    }

    private void UpdateProfileUiLocks()
    {
        var std = _activePlayProfile.IsStandard;
        if (VersionCombo != null)
            VersionCombo.IsEnabled = std;
        if (LoaderCombo != null)
            LoaderCombo.IsEnabled = std;
        if (ReleaseFilter != null)
            ReleaseFilter.IsEnabled = std;
        if (InstalledFilter != null)
            InstalledFilter.IsEnabled = std;
        if (AllFilter != null)
            AllFilter.IsEnabled = std;
    }

    private void UpdateActiveTargetFromProfile()
    {
        var p = _activePlayProfile;
        if (ActiveTargetTitle != null)
            ActiveTargetTitle.Text = p.Title;
        if (ActiveTargetSubtitle != null)
        {
            ActiveTargetSubtitle.Text = p.IsStandard
                ? "Default game directory (.minecraft)."
                : $"{p.MinecraftVersion} · {p.Loader}" +
                  (string.IsNullOrWhiteSpace(p.LoaderVersion) ? "" : " " + p.LoaderVersion) +
                  " · mods from instance folder";
        }

        if (ActiveTargetStatus != null)
            ActiveTargetStatus.Text = p.IsStandard ? string.Empty : p.GameDirectory ?? string.Empty;
    }

    private async Task ApplyModpackSelectionAsync(PlayProfile p)
    {
        if (p.IsStandard || VersionCombo == null || LoaderCombo == null)
            return;

        try
        {
            AllFilter.IsChecked = true;
            ReleaseFilter.IsChecked = false;
            InstalledFilter.IsChecked = false;
            _versionFilter = "All";
            await LoadVersionsAsync();

            await Dispatcher.UIThread.InvokeAsync(() =>
            {
                if (!string.IsNullOrWhiteSpace(p.MinecraftVersion))
                    VersionCombo.SelectedItem = p.MinecraftVersion;
                SelectLoaderComboItem(p.Loader);
                UpdateSelectionSummary();
                UpdateLoaderMessage();
                UpdateActiveTargetFromProfile();
            });
        }
        catch
        {
            // ignore UI sync failures
        }
    }

    private void SelectLoaderComboItem(string loader)
    {
        if (LoaderCombo == null)
            return;
        var display = NormalizeLoaderForCombo(loader);
        foreach (var o in LoaderCombo.Items)
        {
            if (o is ComboBoxItem item &&
                string.Equals(item.Content?.ToString(), display, StringComparison.OrdinalIgnoreCase))
            {
                LoaderCombo.SelectedItem = item;
                return;
            }
        }

        foreach (var o in LoaderCombo.Items)
        {
            if (o is ComboBoxItem first)
            {
                LoaderCombo.SelectedItem = first;
                return;
            }
        }
    }

    private static string NormalizeLoaderForCombo(string loader)
    {
        if (string.IsNullOrWhiteSpace(loader))
            return "Fabric";
        if (loader.Equals("Forge", StringComparison.OrdinalIgnoreCase))
            return "Forge";
        if (loader.Equals("Vanilla", StringComparison.OrdinalIgnoreCase))
            return "Vanilla";
        return "Fabric";
    }

    private async void RefreshModpacksButton_Click(object? sender, RoutedEventArgs e)
    {
        await LoadModpacks();
    }

    private async Task LoadModpacks()
    {
        _modpackLoadCts?.Cancel();
        _modpackLoadCts = new CancellationTokenSource();
        var token = _modpackLoadCts.Token;

        try
        {
            UpdateStatus("Downloading Modrinth catalog...");
            _modpacks = new List<ModpackItem>();
            await Dispatcher.UIThread.InvokeAsync(() =>
            {
                _displayedModpacks.Clear();
            });

            const int limit = 100;
            var offset = 0;
            int? totalHits = null;
            var facets = Uri.EscapeDataString("[[\"project_type:modpack\"]]");

            while (!token.IsCancellationRequested)
            {
                var url = $"https://api.modrinth.com/v2/search?facets={facets}&limit={limit}&offset={offset}";
                using var response = await _httpClient.GetAsync(url, HttpCompletionOption.ResponseHeadersRead, token);
                response.EnsureSuccessStatusCode();
                await using var stream = await response.Content.ReadAsStreamAsync(token);
                using var doc = await JsonDocument.ParseAsync(stream, cancellationToken: token);

                if (!totalHits.HasValue && doc.RootElement.TryGetProperty("total_hits", out var th))
                    totalHits = th.GetInt32();

                var hits = doc.RootElement.GetProperty("hits");
                var batch = new List<ModpackItem>();
                foreach (var item in hits.EnumerateArray())
                {
                    if (token.IsCancellationRequested) break;

                    var title = item.TryGetProperty("title", out var te) ? te.GetString() ?? "Untitled" : "Untitled";
                    var slug = item.TryGetProperty("slug", out var se) ? se.GetString() ?? string.Empty : string.Empty;
                    var desc = item.TryGetProperty("description", out var de) ? de.GetString() ?? string.Empty : string.Empty;
                    var pid = item.TryGetProperty("project_id", out var pe) ? pe.GetString() ?? string.Empty : string.Empty;
                    var icon = item.TryGetProperty("icon_url", out var ie) && ie.ValueKind != JsonValueKind.Null
                        ? ie.GetString() ?? string.Empty
                        : string.Empty;
                    var downloads = item.TryGetProperty("downloads", out var dwn) ? dwn.GetInt32() : 0;
                    var follows = item.TryGetProperty("follows", out var fl) ? fl.GetInt32() : 0;

                    batch.Add(new ModpackItem
                    {
                        Title       = title,
                        Description = desc,
                        Slug        = slug,
                        ProjectId   = pid,
                        IconUrl     = icon,
                        Downloads   = downloads,
                        Follows     = follows,
                        IsEnabled   = false
                    });
                }

                if (batch.Count == 0)
                    break;

                _modpacks.AddRange(batch);
                offset += batch.Count;

                var loaded = _modpacks.Count;
                var totalText = totalHits.HasValue ? totalHits.Value.ToString("N0") : "…";
                await Dispatcher.UIThread.InvokeAsync(() =>
                {
                    AppendModpacksToView(batch);
                });

                if (totalHits.HasValue && offset >= totalHits.Value)
                    break;
                if (batch.Count < limit)
                    break;
            }

            if (!token.IsCancellationRequested)
            {
                await Dispatcher.UIThread.InvokeAsync(() =>
                {
                    RefreshModpackInstallStates();
                    FilterModpacks();
                    UpdateStatus($"Catalog ready — {_modpacks.Count:N0} modpacks.");
                    RefreshPlayProfiles();
                });
            }
        }
        catch (OperationCanceledException)
        {
            await Dispatcher.UIThread.InvokeAsync(() =>
            {
                RefreshModpackInstallStates();
                FilterModpacks();
                UpdateStatus("Catalog load stopped.");
            });
        }
        catch (Exception ex)
        {
            await Dispatcher.UIThread.InvokeAsync(() => UpdateStatus($"Modpack catalog failed: {ex.Message}"));
        }
    }

    private void AppendModpacksToView(IEnumerable<ModpackItem> batch)
    {
        var term = ContentSearchBox?.Text?.Trim().ToLowerInvariant() ?? "";
        if (string.IsNullOrEmpty(term))
        {
            foreach (var item in batch)
            {
                item.RefreshInstallState(_instancesFolder);
                _displayedModpacks.Add(item);
            }
        }
        else
            FilterModpacks();
    }

    private void RefreshModpackInstallStates()
    {
        foreach (var m in _modpacks)
            m.RefreshInstallState(_instancesFolder);
    }

    private void StopModpackLoad_Click(object? sender, RoutedEventArgs e)
    {
        _modpackLoadCts?.Cancel();
    }

    private async void ModpackRow_Loaded(object? sender, RoutedEventArgs e)
    {
        if (sender is not Control { DataContext: ModpackItem item })
            return;
        try
        {
            await item.TryLoadIconAsync(_httpClient);
        }
        catch
        {
            // ignore icon failures
        }
    }

    private async void ModpackInstall_Click(object? sender, RoutedEventArgs e)
    {
        if (sender is not Button { DataContext: ModpackItem pack })
            return;
        if (!pack.CanClickInstall)
            return;

        var projectRef = !string.IsNullOrWhiteSpace(pack.ProjectId) ? pack.ProjectId : pack.Slug;
        if (string.IsNullOrWhiteSpace(projectRef))
            return;

        pack.SetInstalling(true);
        try
        {
            using var cts = new CancellationTokenSource(TimeSpan.FromMinutes(45));
            var token = cts.Token;
            var destFolder = Path.Combine(_instancesFolder, ModpackItem.SanitizeFolderName(pack.PickFolderKey()));

            UpdateStatus($"Looking up {pack.Title}…");
            var fileUrl = await ModrinthModpackInstaller.TryGetLatestMrpackUrlAsync(_httpClient, projectRef, token);
            if (string.IsNullOrWhiteSpace(fileUrl))
            {
                UpdateStatus($"No .mrpack file listed for {pack.Title}.");
                return;
            }

            var temp = Path.Combine(Path.GetTempPath(), $"xylar-{Guid.NewGuid():N}.mrpack");
            try
            {
                UpdateStatus($"Downloading {pack.Title}…");
                await ModrinthModpackInstaller.DownloadFileAsync(_httpClient, fileUrl, temp, token);
                UpdateStatus($"Extracting {pack.Title}…");
                ModrinthModpackInstaller.ExtractMrpackZip(temp, destFolder);
                var packRoot = ModrinthModpackInstaller.FindPackRoot(destFolder);
                if (packRoot != null)
                {
                    UpdateStatus($"Downloading mods for {pack.Title} (Modrinth index)…");
                    var progress = new Progress<(int done, int total, string relativePath)>(p =>
                    {
                        _ = Dispatcher.UIThread.InvokeAsync(() =>
                        {
                            if (DownloadProgress != null && p.total > 0)
                                DownloadProgress.Value = 100.0 * p.done / Math.Max(1, p.total);
                            UpdateStatus(p.done == 0
                                ? $"Preparing files… ({p.total} entries)"
                                : $"Mods {p.done}/{p.total}: {p.relativePath}");
                        });
                    });
                    await ModrinthModpackInstaller.DownloadPackFilesFromIndexAsync(_httpClient, packRoot, progress, token);
                }

                pack.MarkInstalled(destFolder);
                RefreshPlayProfiles();
                UpdateStatus($"Installed {pack.Title} (mods + index) under instances\\{Path.GetFileName(destFolder)}");
            }
            finally
            {
                try
                {
                    if (File.Exists(temp))
                        File.Delete(temp);
                }
                catch
                {
                    // ignore
                }
            }
        }
        catch (Exception ex)
        {
            UpdateStatus($"Install failed: {ex.Message}");
        }
        finally
        {
            pack.SetInstalling(false);
        }
    }

    private void ModpackWeb_Click(object? sender, RoutedEventArgs e)
    {
        if (sender is not Button { DataContext: ModpackItem pack })
            return;

        var slug = string.IsNullOrWhiteSpace(pack.Slug) ? pack.ProjectId : pack.Slug;
        if (string.IsNullOrWhiteSpace(slug))
            return;

        var url = $"https://modrinth.com/modpack/{slug}";
        Process.Start(new ProcessStartInfo { FileName = url, UseShellExecute = true });
        UpdateStatus($"Opened {pack.Title} in browser.");
    }

    private void ModpackOpenFolder_Click(object? sender, RoutedEventArgs e)
    {
        if (sender is not Button { DataContext: ModpackItem pack })
            return;
        if (string.IsNullOrWhiteSpace(pack.InstancePath) || !Directory.Exists(pack.InstancePath))
            return;

        Process.Start(new ProcessStartInfo { FileName = pack.InstancePath, UseShellExecute = true });
        UpdateStatus($"Opened folder for {pack.Title}.");
    }

    private void ModpackList_SelectionChanged(object? sender, SelectionChangedEventArgs e)
    {
    }

    private void ModpackToggle_Checked(object? sender, RoutedEventArgs e)
        => HandleModpackToggle(sender, true);

    private void ModpackToggle_Unchecked(object? sender, RoutedEventArgs e)
        => HandleModpackToggle(sender, false);

    private void HandleModpackToggle(object? sender, bool enabled)
    {
        if (sender is ToggleSwitch ts && ts.DataContext is ModpackItem item)
        {
            item.IsEnabled = enabled;
            UpdateStatus(enabled ? $"Marked modpack: {item.Title}" : $"Unmarked modpack: {item.Title}");
        }
    }

    private void OpenOfflineSkinsModPage_Click(object? sender, RoutedEventArgs e)
    {
        Process.Start(new ProcessStartInfo
        {
            FileName = "https://modrinth.com/mod/offlineskins?version=1.21.11",
            UseShellExecute = true
        });
        UpdateStatus("Opened OfflineSkins on Modrinth.");
    }

    private void DiscordXylarInc_Click(object? sender, RoutedEventArgs e)
    {
        Process.Start(new ProcessStartInfo { FileName = "https://discord.gg/CQ6Et3Kws7", UseShellExecute = true });
        UpdateStatus("Opened Xylar Inc. Discord.");
    }

    private void DiscordXylarSupport_Click(object? sender, RoutedEventArgs e)
    {
        Process.Start(new ProcessStartInfo { FileName = "https://discord.gg/KEHccUvvaX", UseShellExecute = true });
        UpdateStatus("Opened Xylar Support Discord.");
    }

    private void LoadEmbeddedBrandAssets()
    {
        var appDir = AppContext.BaseDirectory;
        System.IO.File.AppendAllText("debug.log", $"Loading embedded brand assets from: {appDir}\n");

        try
        {
            if (AboutLogoXylarInc != null)
            {
                var path = Path.Combine(appDir, "Resources", "logo", "xylarinc.png");
                System.IO.File.AppendAllText("debug.log", $"Trying to load xylarinc from: {path}\n");
                if (File.Exists(path))
                {
                    AboutLogoXylarInc.Source = new Bitmap(path);
                    System.IO.File.AppendAllText("debug.log", "xylarinc.png loaded successfully\n");
                }
                else
                    System.IO.File.AppendAllText("debug.log", $"File not found: {path}\n");
            }
        }
        catch (Exception ex) { System.IO.File.AppendAllText("debug.log", $"Error loading xylarinc.png: {ex}\n"); }

        try
        {
            if (AboutLogoXylarSupport != null)
            {
                var path = Path.Combine(appDir, "Resources", "logo", "xylarsupport.png");
                System.IO.File.AppendAllText("debug.log", $"Trying to load xylarsupport from: {path}\n");
                if (File.Exists(path))
                {
                    AboutLogoXylarSupport.Source = new Bitmap(path);
                    System.IO.File.AppendAllText("debug.log", "xylarsupport.png loaded successfully\n");
                }
                else
                    System.IO.File.AppendAllText("debug.log", $"File not found: {path}\n");
            }
        }
        catch (Exception ex) { System.IO.File.AppendAllText("debug.log", $"Error loading xylarsupport.png: {ex}\n"); }

        try
        {
            if (SidebarBrandImage != null)
            {
                var path = Path.Combine(appDir, "Assets", "minecraft.png");
                if (File.Exists(path))
                    SidebarBrandImage.Source = new Bitmap(path);
            }
        }
        catch (Exception ex) { System.IO.File.AppendAllText("debug.log", $"Error loading sidebar logo: {ex}\n"); }

        try
        {
            if (ProfileAvatarImage != null)
            {
                UpdateProfileAvatar();
            }
        }
        catch (Exception ex) { System.IO.File.AppendAllText("debug.log", $"Error loading profile avatar: {ex}\n"); }

        try
        {
            var iconPath = Path.Combine(appDir, "minecraft.ico");
            if (!File.Exists(iconPath))
                iconPath = Path.Combine(appDir, "Assets", "minecraft.png");
            if (File.Exists(iconPath))
                Icon = new WindowIcon(iconPath);
        }
        catch (Exception ex) { System.IO.File.AppendAllText("debug.log", $"Error loading icon: {ex}\n"); }
    }

    private static Bitmap? OpenBitmapAsset(string assemblyName, string relativePath)
    {
        try
        {
            var uri = new Uri($"avares://{assemblyName}/{relativePath}");
            using var input = AssetLoader.Open(uri);
            using var ms = new MemoryStream();
            input.CopyTo(ms);
            ms.Position = 0;
            return new Bitmap(ms);
        }
        catch
        {
            return null;
        }
    }

    private void UpdateProfileAvatar()
    {
        try
        {
            if (ProfileAvatarImage == null) return;

            string username = UsernameBox?.Text?.Trim() ?? DefaultOfflineUsername;
            string skinPath = Path.Combine(_skinsFolder, $"{username}.png");

            if (File.Exists(skinPath))
            {
                ProfileAvatarImage.Source = new Bitmap(skinPath);
            }
            else
            {
                // Fallback to steve.png
                string stevePath = Path.Combine(AppContext.BaseDirectory, "Assets", "steve.png");
                if (File.Exists(stevePath))
                    ProfileAvatarImage.Source = new Bitmap(stevePath);
            }
        }
        catch (Exception ex)
        {
            System.IO.File.AppendAllText("debug.log", $"Error updating profile avatar: {ex}\n");
        }
    }

    private void ModSearchBox_KeyUp(object? sender, Avalonia.Input.KeyEventArgs e)
        => FilterModpacks();

    private void FilterModpacks()
    {
        var searchTerm = ContentSearchBox?.Text?.ToLower() ?? "";
        var filtered = string.IsNullOrWhiteSpace(searchTerm)
            ? _modpacks
            : _modpacks.Where(m =>
                m.Title.ToLower().Contains(searchTerm) ||
                m.Description.ToLower().Contains(searchTerm)).ToList();

        _displayedModpacks.Clear();
        foreach (var item in filtered)
        {
            item.RefreshInstallState(_instancesFolder);
            _displayedModpacks.Add(item);
        }
    }

    private async void NavButton_Click(object? sender, RoutedEventArgs e)
    {
        if (sender is not ToggleButton btn) return;

        if (btn == NavSkins && string.Equals(btn.Content?.ToString(), "Settings", StringComparison.Ordinal))
        {
            SetHeader("Settings", string.Empty);
            NavMain.IsChecked = false;
            NavSkins.IsChecked = true;
            NavMods.IsChecked = false;
            NavCredits.IsChecked = false;
            await AnimateSectionSwitchAsync(OptionsSection);
            return;
        }

        string content = btn.Content?.ToString() ?? "";
        Border? target = content switch
        {
            "Home"     => MainSection,
            "Skins"    => SkinSection,
            "Modspack" => ModSection,
            "About"    => CreditsSection,
            _          => null
        };

        switch (content)
        {
            case "Home":
                SetHeader("Home", string.Empty);
                RefreshPlayProfiles();
                break;
            case "Skins":
                SetHeader("Settings", string.Empty);
                break;
            case "Modspack":
                SetHeader("Modspack", string.Empty);
                UpdateContentBrowserCopy();
                _ = LoadContentBrowserAsync();
                break;
            case "About":
                SetHeader("About", string.Empty);
                break;
        }

        NavMain.IsChecked    = content == "Home";
        NavSkins.IsChecked   = content == "Skins";
        NavMods.IsChecked    = content == "Modspack";
        NavCredits.IsChecked = content == "About";

        if (content == "Home")
            SetHeader("Home", string.Empty);

        await AnimateSectionSwitchAsync(target);
    }

    private async Task AnimateSectionSwitchAsync(Border? target)
    {
        var sections = new[] { MainSection, SkinSection, ModSection, OptionsSection, CreditsSection };
        
        foreach (var s in sections)
        {
            if (s == null) continue;
            if (s != target)
            {
                s.IsVisible = false;
                s.Opacity   = 1;
            }
        }

        if (target == null) return;

        target.Opacity   = 0;
        target.IsVisible = true;
        target.RenderTransform = new TranslateTransform(0, 18);
        target.InvalidateMeasure();
        const int steps = 12;
        for (var i = 1; i <= steps; i++)
        {
            var progress = i / (double)steps;
            target.Opacity = progress;
            if (target.RenderTransform is TranslateTransform translate)
                translate.Y = 18 * (1d - progress);
            await Task.Delay(12);
        }
        target.Opacity = 1;
        if (target.RenderTransform is TranslateTransform settled)
            settled.Y = 0;
    }

    private void SetHeader(string title, string subtitle)
    {
        if (HeaderTitleText != null) HeaderTitleText.Text = title;
        if (HeaderSubtitleText != null) HeaderSubtitleText.Text = subtitle;
    }

    private void LoaderCombo_SelectionChanged(object? sender, SelectionChangedEventArgs e)
    {
        if (!_isInitialized) return;
        UpdateLoaderMessage();
        UpdateActiveTargetFromProfile();
    }

    private void VersionCombo_SelectionChanged(object? sender, SelectionChangedEventArgs e)
    {
        if (!_isInitialized) return;
        UpdateSelectionSummary();
        UpdateActiveTargetFromProfile();
    }

    private void UsernameBox_TextChanged(object? sender, TextChangedEventArgs e)
    {
        if (!_isInitialized) return;
        UpdateSelectionSummary();
    }

    private void ContentTab_Click(object? sender, RoutedEventArgs e)
    {
        _contentFilter.ContentType = ContentType.Modpack;
        _contentPage = 1;
        UpdateContentBrowserCopy();
        _ = LoadContentBrowserAsync(resetList: true);
    }

    private void UpdateContentBrowserCopy()
    {
        if (ContentSearchBox != null)
        {
            ContentSearchBox.Watermark = "Search modpacks, e.g. Better MC";
        }

        if (ContentStatusText != null && string.IsNullOrWhiteSpace(ContentStatusText.Text))
        {
            ContentStatusText.Text = "Browse modpacks and pick a supported Minecraft version before install.";
        }
    }

    private void ContentSearchBox_KeyUp(object? sender, KeyEventArgs e)
    {
        _contentSearchDebounceCts?.Cancel();
        _contentSearchDebounceCts = new CancellationTokenSource();
        var token = _contentSearchDebounceCts.Token;
        _ = DebouncedSearchAsync(token);
    }

    private async Task DebouncedSearchAsync(CancellationToken token)
    {
        try
        {
            await Task.Delay(320, token);
            _contentFilter.SearchQuery = ContentSearchBox?.Text ?? "";
            _contentPage = 1;
            await LoadContentBrowserAsync(resetList: true);
        }
        catch (OperationCanceledException)
        {
            // expected when user continues typing
        }
    }

    private async Task LoadContentBrowserAsync(bool resetList = true)
    {
        _contentLoadCts?.Cancel();
        _contentThumbCts?.Cancel();
        _contentLoadCts = new CancellationTokenSource();
        var token = _contentLoadCts.Token;

        try
        {
            if (ContentLoadingBar == null) return;
            ContentLoadingBar.IsVisible = true;
            ContentLoadingBar.Value = 0;
            UpdateStatus("Loading content browser...");
            if (ContentStatusText != null)
                ContentStatusText.Text = "Loading modpacks from Modrinth...";

            var filter = _contentFilter;
            var result = await _contentService!.SearchAsync(filter, _contentPage, ContentPageSize, token);
            _contentHasMore = result.HasMorePages;
            ContentLoadingBar.Value = 20;

            await Dispatcher.UIThread.InvokeAsync(() =>
            {
                if (ContentItemsControl != null)
                {
                    if (resetList)
                        _contentItems.Clear();

                    var existingKeys = new HashSet<string>(
                        _contentItems.Select(item => $"{item.Source}:{item.Id}"),
                        StringComparer.OrdinalIgnoreCase);

                    var count = Math.Max(1, result.Items.Count);
                    for (var i = 0; i < result.Items.Count; i++)
                    {
                        var item = result.Items[i];
                        var itemKey = $"{item.Source}:{item.Id}";
                        if (!resetList && existingKeys.Contains(itemKey))
                            continue;

                        if (!string.IsNullOrWhiteSpace(item.IconUrl) &&
                            _contentThumbnailCache.TryGetValue(item.IconUrl, out var cached))
                            item.Thumbnail = cached;

                        _contentItems.Add(item);
                        existingKeys.Add(itemKey);
                        // Bouncy progress phase starts from 20% and ends at 100%.
                        ContentLoadingBar.Value = 20 + ((i + 1) * 80.0 / count);
                    }
                    ContentItemsControl.ItemsSource = _contentItems;
                }
                if (ContentStatusText != null)
                    ContentStatusText.Text = _contentItems.Count == 0
                        ? "No modpacks found yet. Try a different search term."
                        : _contentHasMore
                            ? $"{_contentItems.Count} modpacks shown - load more to keep browsing."
                            : $"{_contentItems.Count} modpacks shown";
                if (LoadMoreContentButton != null)
                    LoadMoreContentButton.IsVisible = _contentHasMore;
                ContentLoadingBar.IsVisible = false;
                UpdateStatus($"Loaded {_contentItems.Count} items from {string.Join(" + ", filter.SelectedSources.Select(s => s.GetDisplayName()))}");
            });

            _contentThumbCts = new CancellationTokenSource();
            _ = WarmThumbnailsAsync(result.Items, _contentThumbCts.Token);
        }
        catch (OperationCanceledException)
        {
            if (ContentLoadingBar != null)
                ContentLoadingBar.IsVisible = false;
        }
        catch (Exception ex)
        {
            if (ContentLoadingBar != null) ContentLoadingBar.IsVisible = false;
            UpdateStatus($"Content load error: {ex.Message}");
        }
    }

    private void RefreshContentButton_Click(object? sender, RoutedEventArgs e)
    {
        _contentPage = 1;
        _ = LoadContentBrowserAsync(resetList: true);
    }

    private void StopContentLoad_Click(object? sender, RoutedEventArgs e)
    {
        _contentLoadCts?.Cancel();
        _contentThumbCts?.Cancel();
    }

    // Details UI is now removed - simplified install flow
    private void SourceFilter_Click(object? sender, RoutedEventArgs e)
    {
        _contentFilter.SelectedSources.Clear();
        _contentFilter.SelectedSources.Add(ContentSource.Modrinth);
        _contentPage = 1;
        _ = LoadContentBrowserAsync(resetList: true);
    }

    private void ContentOpenPage_Click(object? sender, RoutedEventArgs e)
    {
        if (sender is not Button { DataContext: ContentItem item })
            return;
        if (string.IsNullOrWhiteSpace(item.PageUrl))
            return;

        Process.Start(new ProcessStartInfo { FileName = item.PageUrl, UseShellExecute = true });
        UpdateStatus($"Opened {item.Title} in browser.");
    }

    private async void ContentViewDetails_Click(object? sender, RoutedEventArgs e)
    {
        if (sender is not Control { DataContext: ContentItem item })
            return;

        await OpenInstallOverlayAsync(item);
    }

    private async Task OpenInstallOverlayAsync(ContentItem item)
    {
        var details = MergeContentDetails(item, await _contentService!.GetDetailsAsync(item.Id, item.Source));
        _currentInstallVersions = details.Source == ContentSource.Modrinth
            ? await ModrinthAPI.GetProjectVersions(details.Id) ?? new List<ModrinthAPI.ModVersion>()
            : new List<ModrinthAPI.ModVersion>();
        ShowDetailOverlay(details);
        RefreshInstallSheet();
    }

    private static ContentItem MergeContentDetails(ContentItem summary, ContentItem? details)
    {
        if (details == null)
            return summary;

        if (string.IsNullOrWhiteSpace(details.Author) || string.Equals(details.Author, "Unknown", StringComparison.OrdinalIgnoreCase))
            details.Author = summary.Author;
        if (string.IsNullOrWhiteSpace(details.Description))
            details.Description = summary.Description;
        if (string.IsNullOrWhiteSpace(details.IconUrl))
            details.IconUrl = summary.IconUrl;
        if (details.Thumbnail == null)
            details.Thumbnail = summary.Thumbnail;
        if (details.LastUpdated == DateTime.MinValue)
            details.LastUpdated = summary.LastUpdated;
        if (!details.Categories.Any() && summary.Categories.Any())
            details.Categories = new List<string>(summary.Categories);
        if (!details.Loaders.Any() && summary.Loaders.Any())
            details.Loaders = new List<ModLoader>(summary.Loaders);
        if (!details.MinecraftVersions.Any() && summary.MinecraftVersions.Any())
            details.MinecraftVersions = new List<string>(summary.MinecraftVersions);

        return details;
    }

    private void ShowDetailOverlay(ContentItem item)
    {
        _currentDetailsItem = item;

        if (DetailTitleText != null)
            DetailTitleText.Text = $"Install {item.DisplayType}";
        if (DetailItemTitle != null)
            DetailItemTitle.Text = item.Title;
        if (DetailItemDownloads != null)
            DetailItemDownloads.Text = $"{item.DownloadCount:N0} downloads";
        if (DetailSourceText != null)
            DetailSourceText.Text = item.Source.GetDisplayName();
        if (DetailBodyTitle != null)
            DetailBodyTitle.Text = "Install";

        if (DetailIconImage != null)
            DetailIconImage.Source = item.Thumbnail;

        if (DetailLinkProject != null)
            DetailLinkProject.Text = $"Project page: {item.PageUrl}";

        if (DetailVersionTags != null)
        {
            DetailVersionTags.Children.Clear();
            foreach (var version in GetDisplayedVersions(item).Take(10))
            {
                DetailVersionTags.Children.Add(new Border
                {
                    Background = new SolidColorBrush(Color.Parse("#343949")),
                    CornerRadius = new CornerRadius(4),
                    Padding = new Thickness(8, 4, 8, 4),
                    Margin = new Thickness(0, 0, 6, 4),
                    Child = new TextBlock { Text = version, FontSize = 12 }
                });
            }
        }

        if (DetailPlatformTags != null)
        {
            DetailPlatformTags.Children.Clear();
            foreach (var loader in GetDisplayedLoaders(item))
            {
                DetailPlatformTags.Children.Add(new Border
                {
                    Background = new SolidColorBrush(Color.Parse("#343949")),
                    CornerRadius = new CornerRadius(4),
                    Padding = new Thickness(8, 4, 8, 4),
                    Margin = new Thickness(0, 0, 6, 4),
                    Child = new TextBlock { Text = loader, FontSize = 12 }
                });
            }
        }

        if (DetailTagsPanel != null)
        {
            DetailTagsPanel.Children.Clear();
            foreach (var cat in item.Categories.Take(12))
            {
                DetailTagsPanel.Children.Add(new Border
                {
                    Background = new SolidColorBrush(Color.Parse("#343949")),
                    CornerRadius = new CornerRadius(4),
                    Padding = new Thickness(8, 4, 8, 4),
                    Margin = new Thickness(0, 0, 6, 4),
                    Child = new TextBlock { Text = cat, FontSize = 12 }
                });
            }
        }

        if (DetailBodyText != null)
        {
            DetailBodyText.Text = "Select version and loader.";
        }

        if (DetailInstallStatus != null)
            DetailInstallStatus.Text = "";
        if (DetailInstallProgress != null)
        {
            DetailInstallProgress.IsVisible = false;
            DetailInstallProgress.Value = 0;
        }
        if (DetailInstallButton != null)
            DetailInstallButton.IsEnabled = true;
        if (DetailLoaderPanel != null)
            DetailLoaderPanel.IsVisible = item.ContentType != ContentType.ResourcePack;

        if (DetailOverlay != null)
            DetailOverlay.IsVisible = true;
    }

    private void DetailLinkProject_PointerPressed(object? sender, PointerPressedEventArgs e)
    {
        if (_currentDetailsItem != null && !string.IsNullOrWhiteSpace(_currentDetailsItem.PageUrl))
        {
            Process.Start(new ProcessStartInfo { FileName = _currentDetailsItem.PageUrl, UseShellExecute = true });
        }
    }

    private List<string> GetDisplayedVersions(ContentItem item)
    {
        var versions = _currentInstallVersions
            .SelectMany(version => version.GameVersions ?? new List<string>())
            .Where(version => !string.IsNullOrWhiteSpace(version))
            .Distinct(StringComparer.OrdinalIgnoreCase)
            .ToList();

        if (versions.Count > 0)
            return OrderMinecraftVersions(versions);

        return item.MinecraftVersions.Any()
            ? OrderMinecraftVersions(item.MinecraftVersions)
            : new List<string>();
    }

    private List<string> GetDisplayedLoaders(ContentItem item)
    {
        var loaders = _currentInstallVersions
            .SelectMany(version => version.Loaders ?? new List<string>())
            .Where(loader => !string.IsNullOrWhiteSpace(loader))
            .Select(NormalizeLoaderDisplayName)
            .Where(loader => !string.IsNullOrWhiteSpace(loader))
            .Distinct(StringComparer.OrdinalIgnoreCase)
            .ToList();

        if (loaders.Count > 0)
            return loaders;

        if (item.ContentType == ContentType.ResourcePack)
            return new List<string> { "Vanilla" };

        return item.Loaders.Any()
            ? item.Loaders
                .Select(loader => loader.GetDisplayName())
                .Where(IsSupportedLoaderDisplayName)
                .Distinct(StringComparer.OrdinalIgnoreCase)
                .ToList()
            : new List<string> { "Vanilla" };
    }

    private void DetailVersionCombo_SelectionChanged(object? sender, SelectionChangedEventArgs e)
    {
        if (_isRefreshingInstallSheet)
            return;
        RefreshInstallSheet();
    }

    private void DetailLoaderCombo_SelectionChanged(object? sender, SelectionChangedEventArgs e)
    {
        if (_isRefreshingInstallSheet)
            return;
        RefreshInstallSheetStatus();
    }

    private void RefreshInstallSheet()
    {
        if (_currentDetailsItem == null || DetailVersionCombo == null)
            return;

        _isRefreshingInstallSheet = true;
        try
        {
            var compatibleVersions = GetDisplayedVersions(_currentDetailsItem)
                .Where(IsGameVersionDownloaded)
                .ToList();

            DetailVersionCombo.ItemsSource = compatibleVersions;
            DetailVersionCombo.SelectedItem = compatibleVersions.FirstOrDefault();

            if (DetailLoaderPanel != null)
                DetailLoaderPanel.IsVisible = _currentDetailsItem.ContentType != ContentType.ResourcePack;

            var selectedVersion = compatibleVersions.FirstOrDefault();
            var loaders = GetCompatibleLoaders(_currentDetailsItem, selectedVersion);
            if (DetailLoaderCombo != null)
            {
                DetailLoaderCombo.ItemsSource = loaders;
                DetailLoaderCombo.SelectedItem = loaders.FirstOrDefault();
            }

            if (DetailRequirementText != null)
            {
                DetailRequirementText.Text = compatibleVersions.Count > 0
                    ? "Choose one downloaded version."
                    : "Download a supported version from Home first.";
            }
        }
        finally
        {
            _isRefreshingInstallSheet = false;
        }

        RefreshInstallSheetStatus();
    }

    private void RefreshInstallSheetStatus()
    {
        if (_currentDetailsItem == null)
            return;

        var selectedVersion = DetailVersionCombo?.SelectedItem?.ToString();
        var selectedLoader = GetSelectedInstallLoader();
        var hasVersion = !string.IsNullOrWhiteSpace(selectedVersion);
        var selectedFile = hasVersion ? GetSelectedProjectVersion(_currentDetailsItem, selectedVersion!, selectedLoader) : null;
        var target = hasVersion
            ? ResolveInstallTarget(_currentDetailsItem, selectedVersion!, selectedLoader)
            : (_minecraftPath.BasePath, "Standard profile (.minecraft)");

        _currentInstallTargetPath = target.Item1;
        _currentInstallTargetLabel = target.Item2;

        if (DetailTargetText != null)
            DetailTargetText.Text = _currentInstallTargetLabel;

        if (DetailInstallStatus != null)
        {
            DetailInstallStatus.Text = !hasVersion
                ? "Choose a downloaded version."
                : selectedFile == null
                    ? "No file is available for that choice."
                    : "Ready.";
        }

        if (DetailInstallButton != null)
            DetailInstallButton.IsEnabled = hasVersion && selectedFile != null;
    }

    private HashSet<string> GetInstalledMinecraftVersions()
    {
        var versionsDir = Path.Combine(_minecraftPath.BasePath, "versions");
        if (!Directory.Exists(versionsDir))
            return new HashSet<string>(StringComparer.OrdinalIgnoreCase);

        return Directory.GetDirectories(versionsDir)
            .Select(Path.GetFileName)
            .Where(name => !string.IsNullOrWhiteSpace(name))
            .Select(name => name!)
            .ToHashSet(StringComparer.OrdinalIgnoreCase);
    }

    private bool IsGameVersionDownloaded(string? gameVersion)
    {
        if (string.IsNullOrWhiteSpace(gameVersion))
            return false;

        foreach (var installed in GetInstalledMinecraftVersions())
        {
            if (string.Equals(installed, gameVersion, StringComparison.OrdinalIgnoreCase))
                return true;

            if (installed.EndsWith("-" + gameVersion, StringComparison.OrdinalIgnoreCase))
                return true;

            if (installed.Contains(gameVersion, StringComparison.OrdinalIgnoreCase))
                return true;
        }

        return _playProfiles.Any(profile =>
            !string.IsNullOrWhiteSpace(profile.MinecraftVersion) &&
            string.Equals(profile.MinecraftVersion, gameVersion, StringComparison.OrdinalIgnoreCase));
    }

    private List<string> GetCompatibleLoaders(ContentItem item, string? selectedVersion)
    {
        if (item.ContentType == ContentType.ResourcePack)
            return new List<string> { "Vanilla" };

        IEnumerable<string> loaders = _currentInstallVersions
            .Where(version => string.IsNullOrWhiteSpace(selectedVersion) || SupportsGameVersion(version, selectedVersion))
            .SelectMany(version => version.Loaders ?? new List<string>())
            .Where(loader => !string.IsNullOrWhiteSpace(loader))
            .Select(NormalizeLoaderDisplayName)
            .Where(loader => !string.IsNullOrWhiteSpace(loader))
            .Distinct(StringComparer.OrdinalIgnoreCase);

        var list = loaders.ToList();
        if (list.Count > 0)
            return list;

        return GetDisplayedLoaders(item);
    }

    private static bool SupportsGameVersion(ModrinthAPI.ModVersion version, string? gameVersion)
    {
        if (string.IsNullOrWhiteSpace(gameVersion))
            return true;

        return version.GameVersions?.Any(v => string.Equals(v, gameVersion, StringComparison.OrdinalIgnoreCase)) == true;
    }

    private static bool IsSupportedLoaderDisplayName(string loader)
    {
        return loader.Equals("Fabric", StringComparison.OrdinalIgnoreCase)
               || loader.Equals("Forge", StringComparison.OrdinalIgnoreCase)
               || loader.Equals("Vanilla", StringComparison.OrdinalIgnoreCase);
    }

    private static string NormalizeLoaderDisplayName(string loader)
    {
        if (string.IsNullOrWhiteSpace(loader))
            return "Vanilla";

        if (loader.Equals("forge", StringComparison.OrdinalIgnoreCase))
            return "Forge";
        if (loader.Equals("fabric", StringComparison.OrdinalIgnoreCase))
            return "Fabric";
        if (loader.Equals("vanilla", StringComparison.OrdinalIgnoreCase))
            return "Vanilla";

        return string.Empty;
    }

    private static string NormalizeLoaderKey(string? loader)
    {
        return string.IsNullOrWhiteSpace(loader) ? "vanilla" : loader.Trim().ToLowerInvariant();
    }

    private string GetSelectedInstallLoader()
    {
        if (_currentDetailsItem?.ContentType == ContentType.ResourcePack)
            return "Vanilla";

        return DetailLoaderCombo?.SelectedItem?.ToString() ?? "Vanilla";
    }

    private (string, string) ResolveInstallTarget(ContentItem item, string gameVersion, string loader)
    {
        if (item.ContentType == ContentType.Modpack)
            return (Path.Combine(_instancesFolder, SanitizeFolderName(item.Title)), $"New instance: {item.Title}");

        if (_activePlayProfile is { IsStandard: false, GameDirectory: not null } profile &&
            string.Equals(profile.MinecraftVersion, gameVersion, StringComparison.OrdinalIgnoreCase))
        {
            if (item.ContentType == ContentType.ResourcePack ||
                string.Equals(profile.Loader, loader, StringComparison.OrdinalIgnoreCase))
            {
                return (profile.GameDirectory, profile.Title);
            }
        }

        return (_minecraftPath.BasePath, "Standard profile (.minecraft)");
    }

    private ModrinthAPI.ModVersion? GetSelectedProjectVersion(ContentItem item, string gameVersion, string loader)
    {
        var normalizedLoader = NormalizeLoaderKey(loader);

        foreach (var version in _currentInstallVersions)
        {
            if (!SupportsGameVersion(version, gameVersion))
                continue;

            if (item.ContentType != ContentType.ResourcePack)
            {
                var versionLoaders = version.Loaders ?? new List<string>();
                if (!versionLoaders.Any(l => NormalizeLoaderKey(l) == normalizedLoader))
                    continue;
            }

            if (item.ContentType == ContentType.Modpack)
            {
                if (PickPrimaryModpackFile(version) != null)
                    return version;
                continue;
            }

            if (PickPrimaryFile(version) != null)
                return version;
        }

        return null;
    }

    private static ModrinthAPI.ModFile? PickPrimaryFile(ModrinthAPI.ModVersion version)
    {
        var files = version.Files ?? new List<ModrinthAPI.ModFile>();
        return files.FirstOrDefault(file => file.IsPrimary && !string.IsNullOrWhiteSpace(file.Url))
            ?? files.FirstOrDefault(file => !string.IsNullOrWhiteSpace(file.Url));
    }

    private static ModrinthAPI.ModFile? PickPrimaryModpackFile(ModrinthAPI.ModVersion version)
    {
        var files = version.Files ?? new List<ModrinthAPI.ModFile>();
        return files.FirstOrDefault(file =>
                   file.IsPrimary &&
                   !string.IsNullOrWhiteSpace(file.Url) &&
                   !string.IsNullOrWhiteSpace(file.Filename) &&
                   file.Filename.EndsWith(".mrpack", StringComparison.OrdinalIgnoreCase))
               ?? files.FirstOrDefault(file =>
                   !string.IsNullOrWhiteSpace(file.Url) &&
                   !string.IsNullOrWhiteSpace(file.Filename) &&
                   file.Filename.EndsWith(".mrpack", StringComparison.OrdinalIgnoreCase));
    }

    private static List<string> OrderMinecraftVersions(IEnumerable<string> versions)
    {
        return versions
            .Where(version => !string.IsNullOrWhiteSpace(version))
            .Distinct(StringComparer.OrdinalIgnoreCase)
            .OrderByDescending(version => version, StringComparer.OrdinalIgnoreCase)
            .ToList();
    }

    // Details UI is disabled - using simplified install flow
    /*
    private void RenderContentDetails(ContentItem item)
    {
        _currentDetailsItem = item;
        if (DetailsTitleText != null)
            DetailsTitleText.Text = item.Title;
        if (DetailsSummaryText != null)
            DetailsSummaryText.Text = string.IsNullOrWhiteSpace(item.Description) ? "No description available." : item.Description;
        if (DetailsDownloadsText != null)
            DetailsDownloadsText.Text = $"{item.DownloadCount:N0} downloads";
        if (DetailsCompatibilityText != null)
        {
            var versions = item.MinecraftVersions.Any() ? string.Join(", ", item.MinecraftVersions.Take(10)) : "N/A";
            DetailsCompatibilityText.Text = $"Compatibility\nMinecraft: Java Edition\n{versions}";
        }
        if (DetailsPlatformsText != null)
        {
            var loaders = item.Loaders.Any() ? string.Join(", ", item.Loaders.Select(l => l.GetDisplayName())) : "Client and server";
            DetailsPlatformsText.Text = $"Platforms\n{loaders}";
        }
        if (DetailsLinksText != null)
            DetailsLinksText.Text = $"Links\nProject page: {item.PageUrl}";
        if (DetailsTagsText != null)
            DetailsTagsText.Text = $"Tags\n{(item.Categories.Any() ? string.Join(", ", item.Categories.Take(12)) : "No tags")}";
        if (DetailsMetaText != null)
        {
            var updated = item.LastUpdated == DateTime.MinValue ? "Unknown" : item.LastUpdated.ToString("dd MMM yyyy");
            DetailsMetaText.Text = $"Source: {item.Source.GetDisplayName()} · Updated: {updated}";
        }
        RenderDetailsBodyByTab();
    }

    private void DetailsTab_Click(object? sender, RoutedEventArgs e)
    {
        if (DetailsTabDescription != null) DetailsTabDescription.IsChecked = sender == DetailsTabDescription;
        if (DetailsTabChangelog != null) DetailsTabChangelog.IsChecked = sender == DetailsTabChangelog;
        if (DetailsTabVersions != null) DetailsTabVersions.IsChecked = sender == DetailsTabVersions;
        // DetailTabDesc and DetailTabVersions are now commented out in XAML
        RenderDetailsBodyByTab();
    }

    private void RenderDetailsBodyByTab()
    {
        if (DetailsMainBodyText == null || _currentDetailsItem == null)
            return;

        if (DetailsTabChangelog?.IsChecked == true)
        {
            DetailsMainBodyText.Text = "Changelog details are not provided by all sources yet.\n\nTip: use Versions tab to check compatibility quickly.";
            return;
        }

        if (DetailsTabVersions?.IsChecked == true)
        {
            var versions = _currentDetailsItem.MinecraftVersions.Any()
                ? string.Join("\n", _currentDetailsItem.MinecraftVersions.Take(20).Select(v => $"• {v}"))
                : "No version list available.";
            DetailsMainBodyText.Text = versions;
            return;
        }

        DetailsMainBodyText.Text = string.IsNullOrWhiteSpace(_currentDetailsItem.Description)
            ? "No description available."
            : _currentDetailsItem.Description;
    }
    */

    private void LoadMoreContent_Click(object? sender, RoutedEventArgs e)
    {
        if (!_contentHasMore)
            return;
        _contentPage++;
        _ = LoadContentBrowserAsync(resetList: false);
    }

    private async void SettingsMicrosoftSignIn_Click(object? sender, RoutedEventArgs e)
    {
        var launch = await StartMicrosoftSignInAsync();
        if (!launch.Success)
            return;

        _launcherProfile = NormalizeLauncherProfile(LoadLauncherProfile());
        _launcherProfile.AccountMode = AccountModeMicrosoft;
        _launcherProfile.WelcomeCompleted = true;
        _launcherProfile.MicrosoftSignInRequested = true;
        _launcherProfile.MicrosoftDisplayName = launch.DisplayName ?? _launcherProfile.MicrosoftDisplayName;
        SaveLauncherProfile(_launcherProfile);
        ApplyLauncherProfile(_launcherProfile);
        UpdateStatus(BuildMicrosoftLaunchStatus(launch));
    }

    private async void SettingsOfflineMode_Click(object? sender, RoutedEventArgs e)
    {
        await MicrosoftAuth.SignOutAsync();
        _launcherProfile = NormalizeLauncherProfile(LoadLauncherProfile());
        _launcherProfile.AccountMode = AccountModeOffline;
        _launcherProfile.WelcomeCompleted = true;
        _launcherProfile.MicrosoftSignInRequested = false;
        _launcherProfile.MicrosoftDisplayName = string.Empty;
        SaveLauncherProfile(_launcherProfile);
        ApplyLauncherProfile(_launcherProfile);
        UpdateStatus("Offline mode is active.");
    }

    private void OpenLauncherFolder_Click(object? sender, RoutedEventArgs e)
    {
        Process.Start(new ProcessStartInfo { FileName = AppContext.BaseDirectory, UseShellExecute = true });
        UpdateStatus("Opened launcher folder.");
    }

    private void OpenInstancesFolder_Click(object? sender, RoutedEventArgs e)
    {
        Directory.CreateDirectory(_instancesFolder);
        Process.Start(new ProcessStartInfo { FileName = _instancesFolder, UseShellExecute = true });
        UpdateStatus("Opened instances folder.");
    }

    private void OpenMinecraftFolder_Click(object? sender, RoutedEventArgs e)
    {
        Directory.CreateDirectory(_minecraftPath.BasePath);
        Process.Start(new ProcessStartInfo { FileName = _minecraftPath.BasePath, UseShellExecute = true });
        UpdateStatus("Opened Minecraft folder.");
    }

    private void DetailCloseButton_Click(object? sender, RoutedEventArgs e)
    {
        if (DetailOverlay != null)
            DetailOverlay.IsVisible = false;
    }

    private async void DetailInstallButton_Click(object? sender, RoutedEventArgs e)
    {
        if (_currentDetailsItem == null) return;

        var gameVersion = DetailVersionCombo?.SelectedItem?.ToString();
        if (string.IsNullOrWhiteSpace(gameVersion))
        {
            LogToOutput("Install blocked: no downloaded Minecraft version selected.");
            return;
        }

        if (!IsGameVersionDownloaded(gameVersion))
        {
            LogToOutput($"Install blocked: Minecraft {gameVersion} is not downloaded yet.");
            if (DetailInstallStatus != null)
                DetailInstallStatus.Text = $"Download Minecraft {gameVersion} from Home first.";
            return;
        }

        var loader = GetSelectedInstallLoader();
        var selectedVersion = GetSelectedProjectVersion(_currentDetailsItem, gameVersion, loader);
        if (selectedVersion == null)
        {
            LogToOutput($"Install blocked: no compatible file found for {gameVersion} / {loader}.");
            if (DetailInstallStatus != null)
                DetailInstallStatus.Text = "No compatible file is available for that choice.";
            return;
        }

        try
        {
            if (DetailInstallButton != null) DetailInstallButton.IsEnabled = false;
            if (DetailInstallProgress != null) DetailInstallProgress.IsVisible = true;
            if (DetailInstallProgress != null) DetailInstallProgress.Value = 12;
            if (DetailInstallStatus != null) DetailInstallStatus.Text = "Installing...";

            LogToOutput($"Installing {_currentDetailsItem.Title}...");
            LogToOutput($"  Version: {gameVersion}, Loader: {loader}");
            LogToOutput($"  Target: {_currentInstallTargetLabel}");

            Directory.CreateDirectory(_currentInstallTargetPath);
            await DownloadContentAsync(_currentDetailsItem, _currentInstallTargetPath, gameVersion, loader, selectedVersion);

            if (DetailInstallProgress != null) DetailInstallProgress.Value = 100;
            if (DetailInstallStatus != null) DetailInstallStatus.Text = $"Installed in {_currentInstallTargetLabel}.";
            LogToOutput($"Installation complete in {_currentInstallTargetLabel}.");

            RefreshPlayProfiles();
        }
        catch (Exception ex)
        {
            LogToOutput($"Installation failed: {ex.Message}");
            if (DetailInstallStatus != null) DetailInstallStatus.Text = $"Error: {ex.Message}";
        }
        finally
        {
            if (DetailInstallButton != null) DetailInstallButton.IsEnabled = true;
        }
    }

    private async Task DownloadContentAsync(ContentItem item, string targetPath, string gameVersion, string loader, ModrinthAPI.ModVersion selectedVersion)
    {
        if (item.ContentType == ContentType.Modpack)
        {
            await DownloadModpackAsync(item, selectedVersion);
        }
        else if (item.ContentType == ContentType.Mod)
        {
            await DownloadModAsync(item, targetPath, selectedVersion);
        }
        else if (item.ContentType == ContentType.ResourcePack)
        {
            await DownloadResourcePackAsync(item, targetPath, selectedVersion);
        }
    }

    private async Task DownloadModpackAsync(ContentItem item, ModrinthAPI.ModVersion selectedVersion)
    {
        var selectedFile = PickPrimaryModpackFile(selectedVersion);
        if (string.IsNullOrWhiteSpace(selectedFile?.Url))
            throw new Exception("No .mrpack file is available for this modpack version.");

        var instancePath = Path.Combine(_instancesFolder, SanitizeFolderName(item.Title));
        var tempFile = Path.Combine(Path.GetTempPath(), $"xylar-{Guid.NewGuid():N}.mrpack");
        try
        {
            if (DetailInstallProgress != null) DetailInstallProgress.Value = 28;
            await ModrinthModpackInstaller.DownloadFileAsync(_httpClient, selectedFile.Url!, tempFile, CancellationToken.None);
            if (DetailInstallProgress != null) DetailInstallProgress.Value = 46;

            ModrinthModpackInstaller.ExtractMrpackZip(tempFile, instancePath);
            var packRoot = ModrinthModpackInstaller.FindPackRoot(instancePath) ?? instancePath;
            if (DetailInstallProgress != null) DetailInstallProgress.Value = 62;

            var packInfo = ModrinthPackIndexReader.TryRead(packRoot);
            if (packInfo != null &&
                !string.Equals(packInfo.Loader, "Fabric", StringComparison.OrdinalIgnoreCase) &&
                !string.Equals(packInfo.Loader, "Forge", StringComparison.OrdinalIgnoreCase) &&
                !string.Equals(packInfo.Loader, "Vanilla", StringComparison.OrdinalIgnoreCase))
            {
                throw new Exception($"{packInfo.Loader} modpacks are not supported in this build.");
            }

            var progress = new Progress<(int done, int total, string relativePath)>(step =>
            {
                if (DetailInstallProgress != null && step.total > 0)
                    DetailInstallProgress.Value = 62 + (38.0 * step.done / step.total);
                if (DetailInstallStatus != null && step.total > 0)
                    DetailInstallStatus.Text = $"Downloading pack files {step.done}/{step.total}...";
            });

            await ModrinthModpackInstaller.DownloadPackFilesFromIndexAsync(_httpClient, packRoot, progress, CancellationToken.None);
            LogToOutput($"  Modpack installed: {item.Title}");
        }
        finally
        {
            if (File.Exists(tempFile)) File.Delete(tempFile);
        }
    }

    private async Task DownloadModAsync(ContentItem item, string targetPath, ModrinthAPI.ModVersion selectedVersion)
    {
        var selectedFile = PickPrimaryFile(selectedVersion);
        if (string.IsNullOrWhiteSpace(selectedFile?.Url))
            throw new Exception("No compatible file found for this version/loader");

        var modsFolder = Path.Combine(targetPath, "mods");
        Directory.CreateDirectory(modsFolder);

        var fileName = string.IsNullOrWhiteSpace(selectedFile!.Filename)
            ? $"{item.Id}.jar"
            : selectedFile.Filename!;

        var destFile = Path.Combine(modsFolder, fileName);
        if (DetailInstallProgress != null) DetailInstallProgress.Value = 48;
        using var response = await _httpClient.GetAsync(selectedFile.Url!);
        response.EnsureSuccessStatusCode();
        await using var fs = File.Create(destFile);
        await response.Content.CopyToAsync(fs);

        if (DetailInstallProgress != null) DetailInstallProgress.Value = 82;
        LogToOutput("  Checking for conflicts...");
        CheckModConflicts(targetPath);
        LogToOutput($"  Mod downloaded: {fileName}");
    }

    private async Task DownloadResourcePackAsync(ContentItem item, string targetPath, ModrinthAPI.ModVersion selectedVersion)
    {
        var selectedFile = PickPrimaryFile(selectedVersion);
        if (string.IsNullOrWhiteSpace(selectedFile?.Url))
            throw new Exception("No download file available");

        var resourcepacksFolder = Path.Combine(targetPath, "resourcepacks");
        Directory.CreateDirectory(resourcepacksFolder);

        var fileName = string.IsNullOrWhiteSpace(selectedFile!.Filename)
            ? $"{item.Id}.zip"
            : selectedFile.Filename!;

        var destFile = Path.Combine(resourcepacksFolder, fileName);
        if (DetailInstallProgress != null) DetailInstallProgress.Value = 52;
        using var response = await _httpClient.GetAsync(selectedFile.Url!);
        response.EnsureSuccessStatusCode();
        await using var fs = File.Create(destFile);
        await response.Content.CopyToAsync(fs);

        if (DetailInstallProgress != null) DetailInstallProgress.Value = 86;
        LogToOutput($"  Resource pack downloaded: {fileName}");
    }

    private string SanitizeFolderName(string name)
    {
        var invalid = Path.GetInvalidFileNameChars();
        return string.Concat(name.Select(c => invalid.Contains(c) ? '_' : c));
    }

    private LauncherProfile CreateDefaultLauncherProfile()
    {
        return new LauncherProfile
        {
            Username = DefaultOfflineUsername,
            WelcomeCompleted = true,
            AccountMode = AccountModeOffline,
            MicrosoftSignInRequested = false,
            MicrosoftDisplayName = string.Empty
        };
    }

    private LauncherProfile NormalizeLauncherProfile(LauncherProfile profile)
    {
        profile.Username = string.IsNullOrWhiteSpace(profile.Username)
            ? DefaultOfflineUsername
            : profile.Username.Trim();

        profile.AccountMode = string.Equals(profile.AccountMode, AccountModeMicrosoft, StringComparison.OrdinalIgnoreCase)
            ? AccountModeMicrosoft
            : AccountModeOffline;

        profile.MicrosoftDisplayName ??= string.Empty;

        profile.WelcomeCompleted = profile.WelcomeCompleted || !string.IsNullOrWhiteSpace(profile.Username);
        return profile;
    }

    private void ApplyLauncherProfile(LauncherProfile profile)
    {
        _launcherProfile = NormalizeLauncherProfile(profile);

        if (UsernameBox != null)
            UsernameBox.Text = _launcherProfile.Username;

        UpdateProfileAvatar();
        UpdateSelectionSummary();
        UpdateLauncherProfileUi();
    }

    private void UpdateLauncherProfileUi()
    {
        var isMicrosoft = string.Equals(_launcherProfile.AccountMode, AccountModeMicrosoft, StringComparison.OrdinalIgnoreCase);
        var modeText = isMicrosoft ? "Microsoft" : "Offline";
        var accountLabel = GetMicrosoftAccountLabel();
        var accountHeadline = isMicrosoft ? $"Signed in as {accountLabel}" : "Offline mode is active";
        var accountDetails = isMicrosoft
            ? "Microsoft account ready"
            : "Local profile ready";
        var chipBackground = new SolidColorBrush(Color.Parse("#1A1B20"));
        var chipBorder = new SolidColorBrush(Color.Parse("#32323C"));
        var chipText = new SolidColorBrush(Color.Parse("#F0F0F2"));

        // SidebarModeText removed from UI
        if (SettingsAccountHeadlineText != null)
            SettingsAccountHeadlineText.Text = accountHeadline;
        if (SettingsAccountDetailText != null)
            SettingsAccountDetailText.Text = accountDetails;
        if (SettingsAccountModeText != null)
            SettingsAccountModeText.Text = modeText;
        if (SettingsLaunchNameText != null)
            SettingsLaunchNameText.Text = isMicrosoft
                ? $"Account: {accountLabel}"
                : $"Launch name: {_launcherProfile.Username}";
        if (SettingsModeChip != null)
        {
            SettingsModeChip.Background = chipBackground;
            SettingsModeChip.BorderBrush = chipBorder;
        }
        if (SettingsAccountModeText != null)
            SettingsAccountModeText.Foreground = chipText;
    }

    private void PersistMicrosoftIdentity(MicrosoftSignInLaunchResult result)
    {
        if (string.IsNullOrWhiteSpace(result.DisplayName))
            return;

        _launcherProfile.MicrosoftDisplayName = result.DisplayName;
        if (string.Equals(_launcherProfile.AccountMode, AccountModeMicrosoft, StringComparison.OrdinalIgnoreCase))
            SaveLauncherProfile(_launcherProfile);
    }

    private async Task<MicrosoftSignInLaunchResult> StartMicrosoftSignInAsync()
    {
        UpdateStatus("Opening Microsoft sign-in...");
        var launch = await MicrosoftAuth.BeginSignInAsync();

        if (!launch.Success)
        {
            UpdateStatus($"Microsoft sign-in failed: {launch.ErrorMessage}");
        }
        else
        {
            PersistMicrosoftIdentity(launch);
        }

        return launch;
    }

    private static string BuildMicrosoftLaunchStatus(MicrosoftSignInLaunchResult launch)
    {
        return string.IsNullOrWhiteSpace(launch.DisplayName)
            ? "Microsoft sign-in completed."
            : $"Microsoft sign-in completed: {launch.DisplayName}.";
    }

    private async Task RunWelcomeIfNeededAsync()
    {
        var profile = LoadLauncherProfile();
        if (profile.WelcomeCompleted || !string.IsNullOrWhiteSpace(profile.Username))
        {
            ApplyLauncherProfile(profile);
            SaveLauncherProfile(_launcherProfile);
            return;
        }

        var result = await RunInlineWelcomeAsync(isEditMode: false);
        var firstRunProfile = result == null
            ? CreateDefaultLauncherProfile()
            : new LauncherProfile
            {
                Username = string.IsNullOrWhiteSpace(result.Username) ? DefaultOfflineUsername : result.Username.Trim(),
                WelcomeCompleted = true,
                AccountMode = string.Equals(result.AccountMode, AccountModeMicrosoft, StringComparison.OrdinalIgnoreCase)
                    ? AccountModeMicrosoft
                    : AccountModeOffline,
                MicrosoftSignInRequested = result.OpenedMicrosoftSignIn,
                MicrosoftDisplayName = result.MicrosoftDisplayName ?? string.Empty
            };

        ApplyLauncherProfile(firstRunProfile);
        SaveLauncherProfile(_launcherProfile);

        UpdateStatus(result?.OpenedMicrosoftSignIn == true
            ? BuildWelcomeMicrosoftStatus(result)
            : "Offline mode is ready.");
    }

    private static string BuildWelcomeMicrosoftStatus(WelcomeWindowResult? result)
    {
        return string.IsNullOrWhiteSpace(result?.MicrosoftDisplayName)
            ? "Microsoft sign-in completed."
            : $"Microsoft sign-in completed: {result.MicrosoftDisplayName}.";
    }

    private void ProfileMenuButton_Click(object? sender, RoutedEventArgs e)
    {
        // Profile menu button removed from UI
    }

    private async Task<WelcomeWindowResult?> RunInlineWelcomeAsync(bool isEditMode)
    {
        _welcomeEditMode = isEditMode;
        _inlineWelcomeTcs = new TaskCompletionSource<WelcomeWindowResult?>(TaskCreationOptions.RunContinuationsAsynchronously);

        if (isEditMode)
            await ShowWelcomeOfflineStepAsync(isEditMode: true, userValue: UsernameBox?.Text);
        else
            await ShowWelcomeChoiceStepAsync();

        return await _inlineWelcomeTcs.Task;
    }

    private async Task ShowWelcomeChoiceStepAsync()
    {
        if (WelcomeOverlay == null || WelcomeChoicePanel == null || WelcomeInputPanel == null)
            return;

        _welcomeEditMode = false;

        if (WelcomeTitleText != null)
            WelcomeTitleText.Text = "Hi! Welcome to XylarJava!";
        if (WelcomeSubtitleText != null)
            WelcomeSubtitleText.Text = "Continue with Microsoft or stay offline.";
        if (WelcomeModeHintText != null)
            WelcomeModeHintText.Text = "Microsoft or Offline";
        if (WelcomePanelTitleText != null)
            WelcomePanelTitleText.Text = "Choose how to continue";
        if (WelcomePanelSubtitleText != null)
            WelcomePanelSubtitleText.Text = "One tap and you're in.";
        if (WelcomeStatusText != null)
        {
            WelcomeStatusText.Text = string.Empty;
            WelcomeStatusText.IsVisible = false;
        }
        if (WelcomeInputStatusText != null)
        {
            WelcomeInputStatusText.Text = string.Empty;
            WelcomeInputStatusText.IsVisible = false;
        }

        SetWelcomeChoiceButtonsEnabled(true);
        SetWelcomeInputButtonsEnabled(true);

        if (!WelcomeOverlay.IsVisible)
        {
            WelcomeChoicePanel.Opacity = 0;
            WelcomeChoicePanel.IsVisible = true;
            WelcomeInputPanel.IsVisible = false;
            WelcomeOverlay.Opacity = 0;
            WelcomeOverlay.IsVisible = true;
            await FadeControlAsync(WelcomeOverlay, 0, 1, 190);
            await FadeControlAsync(WelcomeChoicePanel, 0, 1, 170);
            return;
        }

        await SwapWelcomePanelsAsync(WelcomeInputPanel, WelcomeChoicePanel);
    }

    private async Task ShowWelcomeOfflineStepAsync(bool isEditMode, string? userValue = null)
    {
        if (WelcomeOverlay == null || WelcomeChoicePanel == null || WelcomeInputPanel == null)
            return;

        _welcomeEditMode = isEditMode;

        if (WelcomeTitleText != null)
            WelcomeTitleText.Text = isEditMode ? "Update launcher name" : "Choose a launcher name";
        if (WelcomeSubtitleText != null)
            WelcomeSubtitleText.Text = isEditMode
                ? "Change the local name used by XylarJava."
                : "Create a local profile and continue.";
        if (WelcomeModeHintText != null)
            WelcomeModeHintText.Text = isEditMode ? "Local profile" : "Offline setup";
        if (WelcomeInputTitleText != null)
            WelcomeInputTitleText.Text = isEditMode ? "Launcher name" : "Offline profile";
        if (WelcomeInputSubtitleText != null)
            WelcomeInputSubtitleText.Text = isEditMode
                ? "Save the name you want to use here."
                : "Pick the name you want to use.";
        if (WelcomeBackButton != null)
            WelcomeBackButton.Content = isEditMode ? "Cancel" : "Back";
        if (WelcomeConfirmButton != null)
            WelcomeConfirmButton.Content = isEditMode ? "Save" : "Continue";
        if (WelcomeInputStatusText != null)
        {
            WelcomeInputStatusText.Text = string.Empty;
            WelcomeInputStatusText.IsVisible = false;
        }

        var normalizedUser = string.IsNullOrWhiteSpace(userValue) ? DefaultOfflineUsername : userValue.Trim();
        if (WelcomeUsernameBox != null)
            WelcomeUsernameBox.Text = normalizedUser;

        SetWelcomeInputButtonsEnabled(true);

        if (!WelcomeOverlay.IsVisible)
        {
            WelcomeInputPanel.Opacity = 0;
            WelcomeInputPanel.IsVisible = true;
            WelcomeChoicePanel.IsVisible = false;
            WelcomeOverlay.Opacity = 0;
            WelcomeOverlay.IsVisible = true;
            await FadeControlAsync(WelcomeOverlay, 0, 1, 190);
            await FadeControlAsync(WelcomeInputPanel, 0, 1, 170);
        }
        else
        {
            await SwapWelcomePanelsAsync(WelcomeChoicePanel, WelcomeInputPanel);
        }

        await Dispatcher.UIThread.InvokeAsync(() =>
        {
            WelcomeUsernameBox?.Focus();
            if (WelcomeUsernameBox != null)
                WelcomeUsernameBox.CaretIndex = WelcomeUsernameBox.Text?.Length ?? 0;
        });
    }

    private async Task CompleteInlineWelcomeAsync(WelcomeWindowResult? result)
    {
        var tcs = _inlineWelcomeTcs;
        _inlineWelcomeTcs = null;

        await HideWelcomeOverlayAsync();

        tcs?.TrySetResult(result);
    }

    private async Task HideWelcomeOverlayAsync()
    {
        if (WelcomeOverlay == null || !WelcomeOverlay.IsVisible)
            return;

        await FadeControlAsync(WelcomeOverlay, WelcomeOverlay.Opacity, 0, 150);
        WelcomeOverlay.IsVisible = false;

        if (WelcomeChoicePanel != null)
        {
            WelcomeChoicePanel.IsVisible = false;
            WelcomeChoicePanel.Opacity = 1;
        }

        if (WelcomeInputPanel != null)
        {
            WelcomeInputPanel.IsVisible = false;
            WelcomeInputPanel.Opacity = 1;
        }
    }

    private async Task SwapWelcomePanelsAsync(Control? hiddenControl, Control? shownControl)
    {
        if (shownControl == null)
            return;

        if (hiddenControl != null && hiddenControl.IsVisible)
        {
            await FadeControlAsync(hiddenControl, hiddenControl.Opacity <= 0 ? 1 : hiddenControl.Opacity, 0, 120);
            hiddenControl.IsVisible = false;
        }

        shownControl.Opacity = 0;
        shownControl.IsVisible = true;
        await FadeControlAsync(shownControl, 0, 1, 170);
    }

    private static async Task FadeControlAsync(Control? control, double from, double to, int durationMs)
    {
        if (control == null)
            return;

        const int steps = 16;
        control.Opacity = from;

        for (int index = 1; index <= steps; index++)
        {
            control.Opacity = from + ((to - from) * index / steps);
            await Task.Delay(Math.Max(1, durationMs / steps));
        }
    }

    private void SetWelcomeChoiceButtonsEnabled(bool enabled)
    {
        if (WelcomeMicrosoftButton != null)
            WelcomeMicrosoftButton.IsEnabled = enabled;

        if (WelcomeOfflineButton != null)
            WelcomeOfflineButton.IsEnabled = enabled;
    }

    private void SetWelcomeInputButtonsEnabled(bool enabled)
    {
        if (WelcomeUsernameBox != null)
            WelcomeUsernameBox.IsEnabled = enabled;

        if (WelcomeBackButton != null)
            WelcomeBackButton.IsEnabled = enabled;

        if (WelcomeConfirmButton != null)
            WelcomeConfirmButton.IsEnabled = enabled;
    }

    private async void WelcomeMicrosoftButton_Click(object? sender, RoutedEventArgs e)
    {
        SetWelcomeChoiceButtonsEnabled(false);
        if (WelcomeStatusText != null)
        {
            WelcomeStatusText.Text = "Opening Microsoft sign-in...";
            WelcomeStatusText.IsVisible = true;
        }

        var launch = await StartMicrosoftSignInAsync();
        if (!launch.Success)
        {
            if (WelcomeStatusText != null)
            {
                WelcomeStatusText.Text = launch.ErrorMessage ?? "Microsoft sign-in failed.";
                WelcomeStatusText.IsVisible = true;
            }
            SetWelcomeChoiceButtonsEnabled(true);
            return;
        }

        await CompleteInlineWelcomeAsync(new WelcomeWindowResult
        {
            Username = string.IsNullOrWhiteSpace(launch.DisplayName) ? DefaultOfflineUsername : launch.DisplayName!,
            AccountMode = AccountModeMicrosoft,
            OpenedMicrosoftSignIn = true,
            MicrosoftStatusMessage = launch.Message,
            MicrosoftDisplayName = launch.DisplayName
        });
    }

    private async void WelcomeOfflineButton_Click(object? sender, RoutedEventArgs e)
    {
        await ShowWelcomeOfflineStepAsync(isEditMode: false, userValue: DefaultOfflineUsername);
    }

    private async void WelcomeBackButton_Click(object? sender, RoutedEventArgs e)
    {
        if (_welcomeEditMode)
        {
            await CompleteInlineWelcomeAsync(null);
            return;
        }

        await ShowWelcomeChoiceStepAsync();
    }

    private async void WelcomeConfirmButton_Click(object? sender, RoutedEventArgs e)
    {
        SetWelcomeInputButtonsEnabled(false);

        var value = WelcomeUsernameBox?.Text?.Trim();
        if (string.IsNullOrWhiteSpace(value))
            value = DefaultOfflineUsername;

        if (WelcomeInputStatusText != null)
        {
            WelcomeInputStatusText.Text = _welcomeEditMode ? "Saving launcher name..." : "Creating local profile...";
            WelcomeInputStatusText.IsVisible = true;
        }

        await CompleteInlineWelcomeAsync(new WelcomeWindowResult
        {
            Username = value,
            AccountMode = _welcomeEditMode ? _launcherProfile.AccountMode : AccountModeOffline,
            OpenedMicrosoftSignIn = false,
            MicrosoftDisplayName = _launcherProfile.MicrosoftDisplayName
        });
    }

    private async void ChangeUsernameFromMenu_Click(object? sender, RoutedEventArgs e)
    {
        var result = await RunInlineWelcomeAsync(isEditMode: true);
        if (result == null)
            return;

        var updatedProfile = new LauncherProfile
        {
            Username = string.IsNullOrWhiteSpace(result.Username) ? DefaultOfflineUsername : result.Username.Trim(),
            WelcomeCompleted = true,
            AccountMode = _launcherProfile.AccountMode,
            MicrosoftSignInRequested = _launcherProfile.MicrosoftSignInRequested,
            MicrosoftDisplayName = _launcherProfile.MicrosoftDisplayName
        };

        ApplyLauncherProfile(updatedProfile);
        SaveLauncherProfile(_launcherProfile);
        UpdateStatus("Launcher name updated.");
    }

    private void ExitButtonFromMenu_Click(object? sender, RoutedEventArgs e)
    {
        Close();
    }

    private void OpenSettingsFromMenu(string statusMessage)
    {
        SetHeader("Settings", string.Empty);
        NavMain.IsChecked = false;
        NavSkins.IsChecked = true;
        NavMods.IsChecked = false;
        NavCredits.IsChecked = false;
        _ = AnimateSectionSwitchAsync(OptionsSection);
        UpdateStatus(statusMessage);
    }

    private void GameDirectoryOption_Click(object? sender, RoutedEventArgs e)
    {
        OpenSettingsFromMenu("Opened Settings.");
    }

    private void JavaSettingsOption_Click(object? sender, RoutedEventArgs e)
    {
        OpenSettingsFromMenu("Opened Settings.");
    }

    private void InterfaceSettingsOption_Click(object? sender, RoutedEventArgs e)
    {
        OpenSettingsFromMenu("Opened Settings.");
    }

    private void ContentSearch_Click(object? sender, RoutedEventArgs e)
    {
        _contentFilter.SearchQuery = ContentSearchBox?.Text ?? string.Empty;
        _contentPage = 1;
        _ = LoadContentBrowserAsync(resetList: true);
    }

    private async void InstallMod_Click(object? sender, RoutedEventArgs e)
    {
        if (sender is not Control { DataContext: ContentItem item })
            return;

        await OpenInstallOverlayAsync(item);
    }

    private LauncherProfile LoadLauncherProfile()
    {
        try
        {
            if (!File.Exists(_profileSettingsFile))
                return new LauncherProfile();
            var json = File.ReadAllText(_profileSettingsFile);
            return JsonSerializer.Deserialize<LauncherProfile>(json) ?? new LauncherProfile();
        }
        catch
        {
            return new LauncherProfile();
        }
    }

    private void SaveLauncherProfile(LauncherProfile profile)
    {
        try
        {
            var json = JsonSerializer.Serialize(profile, new JsonSerializerOptions { WriteIndented = true });
            File.WriteAllText(_profileSettingsFile, json);
        }
        catch
        {
            // best effort save
        }
    }

    private async Task WarmThumbnailsAsync(IEnumerable<ContentItem> items, CancellationToken token)
    {
        // Fill above-the-fold first and avoid blocking the main list render.
        var queue = items.Where(i => i.Thumbnail == null && !string.IsNullOrWhiteSpace(i.IconUrl))
                         .Take(24)
                         .ToList();
        var tasks = queue.Select(item => LoadThumbnailForItemAsync(item, token));
        await Task.WhenAll(tasks);
    }

    private async Task LoadThumbnailForItemAsync(ContentItem item, CancellationToken token)
    {
        if (string.IsNullOrWhiteSpace(item.IconUrl))
            return;
        if (_contentThumbnailCache.TryGetValue(item.IconUrl, out var ready))
        {
            item.Thumbnail = ready;
            return;
        }

        await _thumbnailConcurrency.WaitAsync(token);
        try
        {
            if (_contentThumbnailCache.TryGetValue(item.IconUrl, out var cached))
            {
                item.Thumbnail = cached;
                return;
            }

            var bytes = await _httpClient.GetByteArrayAsync(item.IconUrl, token);
            using var ms = new MemoryStream(bytes);
            var bitmap = new Bitmap(ms);
            _contentThumbnailCache[item.IconUrl] = bitmap;
            await Dispatcher.UIThread.InvokeAsync(() => item.Thumbnail = bitmap);
        }
        catch
        {
            // Ignore single image failure; keep card visible without picture.
        }
        finally
        {
            _thumbnailConcurrency.Release();
        }
    }

    private bool CheckJava21Oracle()
    {
        try
        {
            var javaHome = Environment.GetEnvironmentVariable("JAVA_HOME");
            if (!string.IsNullOrWhiteSpace(javaHome))
            {
                var javaExe = Path.Combine(javaHome, "bin", "java" + (OperatingSystem.IsWindows() ? ".exe" : ""));
                if (File.Exists(javaExe))
                {
                    var version = GetJavaVersion(javaExe);
                    if (version.StartsWith("21") && IsOracleJava(javaHome))
                        return true;
                }
            }

            var pathVar = Environment.GetEnvironmentVariable("PATH");
            if (!string.IsNullOrWhiteSpace(pathVar))
            {
                var paths = pathVar.Split(Path.PathSeparator);
                foreach (var path in paths)
                {
                    var javaExe = Path.Combine(path, "java" + (OperatingSystem.IsWindows() ? ".exe" : ""));
                    if (File.Exists(javaExe))
                    {
                        var version = GetJavaVersion(javaExe);
                        if (version.StartsWith("21") && IsOracleJava(Path.GetDirectoryName(Path.GetDirectoryName(javaExe)) ?? ""))
                            return true;
                    }
                }
            }

            if (OperatingSystem.IsWindows())
            {
                var programFiles = Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles);
                var javaPath = Path.Combine(programFiles, "Java");
                if (Directory.Exists(javaPath))
                {
                    foreach (var dir in Directory.GetDirectories(javaPath, "jdk-21*"))
                    {
                        var javaExe = Path.Combine(dir, "bin", "java.exe");
                        if (File.Exists(javaExe) && IsOracleJava(dir))
                            return true;
                    }
                }
            }
            else
            {
                var linuxPaths = new[] { "/usr/lib/jvm", "/usr/java", "/opt/java", "/opt/jdk" };
                foreach (var basePath in linuxPaths)
                {
                    if (!Directory.Exists(basePath)) continue;
                    foreach (var dir in Directory.GetDirectories(basePath, "*21*"))
                    {
                        var javaExe = Path.Combine(dir, "bin", "java");
                        if (File.Exists(javaExe) && IsOracleJava(dir))
                            return true;
                    }
                }
            }

            return false;
        }
        catch
        {
            return false;
        }
    }

    private string GetJavaVersion(string javaPath)
    {
        try
        {
            using var process = new Process();
            process.StartInfo.FileName = javaPath;
            process.StartInfo.Arguments = "-version";
            process.StartInfo.RedirectStandardError = true;
            process.StartInfo.RedirectStandardOutput = true;
            process.StartInfo.UseShellExecute = false;
            process.Start();
            var output = process.StandardError.ReadToEnd();
            process.WaitForExit(5000);

            if (string.IsNullOrWhiteSpace(output))
                output = process.StandardOutput.ReadToEnd();

            var match = System.Text.RegularExpressions.Regex.Match(output, @"version\s*""?(\d+)");
            if (match.Success)
                return match.Groups[1].Value;

            return "";
        }
        catch
        {
            return "";
        }
    }

    private bool IsOracleJava(string javaHome)
    {
        try
        {
            var releaseFile = Path.Combine(javaHome, "release");
            if (File.Exists(releaseFile))
            {
                var content = File.ReadAllText(releaseFile);
                return content.Contains("Oracle") || content.Contains("oracle");
            }
            return javaHome.ToLower().Contains("oracle");
        }
        catch
        {
            return false;
        }
    }

    private void ShowJava21Popup()
    {
        UpdateStatus("Oracle Java 21 is required. Install it and restart XylarJava.");
        Close();
    }

    private void LogToOutput(string message)
    {
        Dispatcher.UIThread.InvokeAsync(() =>
        {
            if (LogOutputText != null)
            {
                var timestamp = DateTime.Now.ToString("HH:mm:ss");
                LogOutputText.Text += $"[{timestamp}] {message}\n";
                if (LogScrollViewer != null)
                    LogScrollViewer.ScrollToEnd();
            }
        });
    }

    private void SetLoadingBar(bool visible, double value = 0)
    {
        Dispatcher.UIThread.InvokeAsync(() =>
        {
            if (MainLoadingBar != null)
            {
                MainLoadingBar.IsVisible = visible;
                MainLoadingBar.Value = value;
            }
        });
    }

    private void CheckModConflicts(string instancePath)
    {
        try
        {
            var modsFolder = Path.Combine(instancePath, "mods");
            if (!Directory.Exists(modsFolder)) return;

            var modFiles = Directory.GetFiles(modsFolder, "*.jar");
            var modIds = new Dictionary<string, List<string>>();

            foreach (var modFile in modFiles)
            {
                var fileName = Path.GetFileNameWithoutExtension(modFile);
                var baseName = ExtractModBaseName(fileName);

                if (!modIds.ContainsKey(baseName))
                    modIds[baseName] = new List<string>();
                modIds[baseName].Add(modFile);
            }

            foreach (var kvp in modIds)
            {
                if (kvp.Value.Count > 1)
                {
                    LogToOutput($"Conflict detected: {kvp.Key} has {kvp.Value.Count} versions");
                    var versions = kvp.Value.OrderByDescending(f => File.GetLastWriteTime(f)).ToList();
                    for (var i = 1; i < versions.Count; i++)
                    {
                        var disabledPath = versions[i] + ".disabled";
                        File.Move(versions[i], disabledPath);
                        LogToOutput($"  Disabled: {Path.GetFileName(versions[i])}");
                    }
                }
            }
        }
        catch (Exception ex)
        {
            LogToOutput($"Conflict check error: {ex.Message}");
        }
    }

    private string ExtractModBaseName(string fileName)
    {
        var parts = fileName.Split('-', '_');
        if (parts.Length >= 2)
        {
            var versionIndex = Array.FindIndex(parts, p => char.IsDigit(p[0]));
            if (versionIndex > 0)
                return string.Join("-", parts.Take(versionIndex));
        }
        return fileName;
    }

}

public class SkinSettings
{
    public string Path  { get; set; } = string.Empty;
    public string Model { get; set; } = "classic";
}

public class LauncherProfile
{
    public string Username { get; set; } = string.Empty;
    public bool WelcomeCompleted { get; set; }
    public string AccountMode { get; set; } = "Offline";
    public bool MicrosoftSignInRequested { get; set; }
    public string MicrosoftDisplayName { get; set; } = string.Empty;
}
