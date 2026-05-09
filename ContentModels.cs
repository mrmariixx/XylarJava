using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.Linq;
using System.Runtime.CompilerServices;
using Avalonia.Media.Imaging;

namespace XylarJavaLauncher;

public enum ContentSource
{
    Modrinth,
    CurseForge
}

public enum ContentType
{
    Mod,
    ResourcePack,
    Modpack
}

public enum ModLoader
{
    Fabric,
    Forge,
    Vanilla
}

public enum SortOption
{
    Relevance,
    Popularity,
    Latest,
    Name
}

public class ContentItem : INotifyPropertyChanged
{
    private string _id = string.Empty;
    private string _title = string.Empty;
    private string _description = string.Empty;
    private string _author = string.Empty;
    private string _version = string.Empty;
    private string _pageUrl = string.Empty;
    private string _iconUrl = string.Empty;
    private string _downloadUrl = string.Empty;
    private Bitmap? _thumbnail;
    private long _downloadCount;
    private DateTime _lastUpdated;
    private bool _isInstalled;
    private ContentSource _source;
    private ContentType _contentType;
    private List<ModLoader> _loaders = new();
    private List<string> _categories = new();
    private List<string> _minecraftVersions = new();

    public string Id { get => _id; set => SetProperty(ref _id, value); }
    public string Title { get => _title; set => SetProperty(ref _title, value); }
    public string Description { get => _description; set => SetProperty(ref _description, value); }
    public string Author { get => _author; set => SetProperty(ref _author, value); }
    public string Version { get => _version; set => SetProperty(ref _version, value); }
    public string PageUrl { get => _pageUrl; set => SetProperty(ref _pageUrl, value); }
    public string IconUrl { get => _iconUrl; set => SetProperty(ref _iconUrl, value); }
    public string DownloadUrl { get => _downloadUrl; set => SetProperty(ref _downloadUrl, value); }
    public Bitmap? Thumbnail { get => _thumbnail; set => SetProperty(ref _thumbnail, value); }
    public long DownloadCount { get => _downloadCount; set => SetProperty(ref _downloadCount, value); }
    public DateTime LastUpdated { get => _lastUpdated; set => SetProperty(ref _lastUpdated, value); }
    public bool IsInstalled { get => _isInstalled; set => SetProperty(ref _isInstalled, value); }
    public ContentSource Source { get => _source; set => SetProperty(ref _source, value); }
    public ContentType ContentType { get => _contentType; set => SetProperty(ref _contentType, value); }
    public List<ModLoader> Loaders { get => _loaders; set => SetProperty(ref _loaders, value); }
    public List<string> Categories { get => _categories; set => SetProperty(ref _categories, value); }
    public List<string> MinecraftVersions { get => _minecraftVersions; set => SetProperty(ref _minecraftVersions, value); }
    public string DisplayType => ContentType switch
    {
        ContentType.Modpack => "Modpack",
        ContentType.ResourcePack => "Resource Pack",
        _ => "Mod"
    };
    public string DisplaySourceName => Source.GetDisplayName();
    public string DisplayAuthor => string.IsNullOrWhiteSpace(Author) ? "Unknown author" : $"by {Author}";
    public string DisplayUpdated => LastUpdated == DateTime.MinValue ? "Updated recently" : $"Updated {LastUpdated:dd MMM yyyy}";
    public string DisplayPrimaryCategory => Categories.FirstOrDefault() is { Length: > 0 } value
        ? HumanizeToken(value)
        : "Minecraft Java";
    public string DisplayLoaderSummary => Loaders.Any()
        ? string.Join(" / ", Loaders.Take(3).Select(loader => loader.GetDisplayName()))
        : "Vanilla";

    public string GetNormalizedTitle() => Title.ToLowerInvariant().Replace(" ", "-").Replace("_", "-");

    public bool Matches(ContentItem other)
    {
        var thisTitle = GetNormalizedTitle();
        var otherTitle = other.GetNormalizedTitle();
        return thisTitle == otherTitle ||
               thisTitle.Contains(otherTitle) ||
               otherTitle.Contains(thisTitle);
    }

    public event PropertyChangedEventHandler? PropertyChanged;

    protected bool SetProperty<T>(ref T field, T value, [CallerMemberName] string propertyName = "")
    {
        if (EqualityComparer<T>.Default.Equals(field, value)) return false;
        field = value;
        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        return true;
    }

    private static string HumanizeToken(string value)
    {
        var normalized = value.Replace("-", " ").Replace("_", " ").Trim();
        return string.IsNullOrWhiteSpace(normalized)
            ? "Minecraft Java"
            : CultureInfo.InvariantCulture.TextInfo.ToTitleCase(normalized.ToLowerInvariant());
    }
}

public class ModItem : ContentItem
{
    public ModItem()
    {
        ContentType = ContentType.Mod;
    }
}

public class ResourcePackItem : ContentItem
{
    public ResourcePackItem()
    {
        ContentType = ContentType.ResourcePack;
    }
}

public class ModpackContentItem : ContentItem
{
    public ModpackContentItem()
    {
        ContentType = ContentType.Modpack;
    }
}

public class ContentFilter
{
    public string SearchQuery { get; set; } = string.Empty;
    public ContentType ContentType { get; set; } = ContentType.Modpack;
    public List<ContentSource> SelectedSources { get; set; } = new();
    public List<ModLoader> SelectedLoaders { get; set; } = new();
    public List<string> SelectedVersions { get; set; } = new();
    public SortOption SortBy { get; set; } = SortOption.Relevance;
    public int Page { get; set; } = 1;
    public int PageSize { get; set; } = 20;
}

public class ContentSearchResult
{
    public List<ContentItem> Items { get; set; } = new();
    public int TotalCount { get; set; }
    public int Page { get; set; }
    public int PageSize { get; set; }
    public bool HasMorePages { get; set; }
}

public static class ContentSourceExtensions
{
    public static string GetDisplayName(this ContentSource source) => source switch
    {
        ContentSource.Modrinth => "Modrinth",
        ContentSource.CurseForge => "CurseForge",
        _ => "Unknown"
    };

    public static string GetColor(this ContentSource source) => source switch
    {
        ContentSource.Modrinth => "#00D084",
        ContentSource.CurseForge => "#F16436",
        _ => "#808080"
    };
}

public static class ModLoaderExtensions
{
    public static string GetDisplayName(this ModLoader loader) => loader switch
    {
        ModLoader.Fabric => "Fabric",
        ModLoader.Forge => "Forge",
        ModLoader.Vanilla => "Vanilla",
        _ => "Unknown"
    };

    public static string GetColor(this ModLoader loader) => loader switch
    {
        ModLoader.Fabric => "#DBB8FF",
        ModLoader.Forge => "#F16436",
        ModLoader.Vanilla => "#808080",
        _ => "#808080"
    };
}
