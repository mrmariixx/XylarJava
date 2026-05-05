using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
using System.Text.Json;
using System.Threading;
using System.Threading.Tasks;
using Avalonia.Media.Imaging;
using HtmlAgilityPack;

namespace XylarJavaLauncher;

public interface IContentParser
{
    ContentSource Source { get; }
    Task<ContentSearchResult> SearchAsync(ContentFilter filter, int page = 1, int pageSize = 20, CancellationToken cancellationToken = default);
    Task<ContentItem?> GetDetailsAsync(string id, CancellationToken cancellationToken = default);
}

public abstract class BaseContentParser : IContentParser
{
    protected readonly HttpClient HttpClient;
    public abstract ContentSource Source { get; }

    protected BaseContentParser(HttpClient httpClient)
    {
        HttpClient = httpClient;
    }

    public abstract Task<ContentSearchResult> SearchAsync(ContentFilter filter, int page = 1, int pageSize = 20, CancellationToken cancellationToken = default);
    public abstract Task<ContentItem?> GetDetailsAsync(string id, CancellationToken cancellationToken = default);

    protected static async Task<Bitmap?> LoadImageAsync(string? url, HttpClient client)
    {
        if (string.IsNullOrWhiteSpace(url)) return null;
        try
        {
            var data = await client.GetByteArrayAsync(url);
            return new Bitmap(new System.IO.MemoryStream(data));
        }
        catch { return null; }
    }
}

public class ModrinthParser : BaseContentParser
{
    private const string ApiBase = "https://api.modrinth.com/v2";
    public override ContentSource Source => ContentSource.Modrinth;

    public ModrinthParser(HttpClient httpClient) : base(httpClient) { }

    public override async Task<ContentSearchResult> SearchAsync(ContentFilter filter, int page = 1, int pageSize = 20, CancellationToken cancellationToken = default)
    {
        pageSize = Math.Clamp(pageSize, 1, 100);

        var facets = new List<List<string>>
        {
            new()
            {
                filter.ContentType switch
                {
                    ContentType.Modpack => "project_type:modpack",
                    ContentType.ResourcePack => "project_type:resourcepack",
                    _ => "project_type:mod"
                }
            }
        };

        if (filter.SelectedLoaders.Any())
        {
            foreach (var loader in filter.SelectedLoaders)
                facets.Add(new List<string> { $"categories:{loader.ToString().ToLowerInvariant()}" });
        }

        if (filter.SelectedVersions.Any())
        {
            foreach (var version in filter.SelectedVersions.Where(v => !string.IsNullOrWhiteSpace(v)))
                facets.Add(new List<string> { $"versions:{version}" });
        }

        var query = Uri.EscapeDataString(filter.SearchQuery ?? "");
        var facetsJson = JsonSerializer.Serialize(facets);
        var facetsParam = Uri.EscapeDataString(facetsJson);
        var index = filter.SortBy switch
        {
            SortOption.Latest => "updated",
            SortOption.Name => "relevance",
            _ when string.IsNullOrWhiteSpace(filter.SearchQuery) => "downloads",
            _ => "relevance"
        };
        var url = $"{ApiBase}/search?query={query}&facets={facetsParam}&index={index}&limit={pageSize}&offset={(page - 1) * pageSize}";
        using var message = new HttpRequestMessage(HttpMethod.Get, url);
        using var response = await HttpClient.SendAsync(message, cancellationToken);
        if (!response.IsSuccessStatusCode)
        {
            return new ContentSearchResult
            {
                Items = new List<ContentItem>(),
                TotalCount = 0,
                Page = page,
                PageSize = pageSize,
                HasMorePages = false
            };
        }
        var payload = await response.Content.ReadAsStringAsync(cancellationToken);
        using var doc = JsonDocument.Parse(payload);
        
        var items = new List<ContentItem>();
        var hits = doc.RootElement.GetProperty("hits");
        
        foreach (var hit in hits.EnumerateArray())
        {
            var item = CreateItemFromHit(hit, filter.ContentType);
            if (item != null) items.Add(item);
        }

        return new ContentSearchResult
        {
            Items = items,
            TotalCount = doc.RootElement.TryGetProperty("total_hits", out var total) ? total.GetInt32() : items.Count,
            Page = page,
            PageSize = pageSize,
            HasMorePages = items.Count == pageSize
        };
    }

    public override async Task<ContentItem?> GetDetailsAsync(string id, CancellationToken cancellationToken = default)
    {
        var url = $"{ApiBase}/project/{id}";
        var json = await HttpClient.GetStringAsync(url, cancellationToken);
        var doc = JsonDocument.Parse(json);
        return CreateItemFromProject(doc.RootElement);
    }

    private ContentItem? CreateItemFromHit(JsonElement hit, ContentType type)
    {
        try
        {
            ContentItem item = type == ContentType.ResourcePack ? new ResourcePackItem() :
                               type == ContentType.Modpack ? new ModpackContentItem() :
                               new ModItem();

            item.Id = hit.GetProperty("project_id").GetString() ?? Guid.NewGuid().ToString();
            item.Title = hit.GetProperty("title").GetString() ?? "Unknown";
            item.Description = hit.GetProperty("description").GetString() ?? string.Empty;
            item.Author = hit.GetProperty("author").GetString() ?? "Unknown";
            var slug = hit.GetProperty("slug").GetString() ?? string.Empty;
            item.PageUrl = type switch
            {
                ContentType.Modpack => $"https://modrinth.com/modpack/{slug}",
                ContentType.ResourcePack => $"https://modrinth.com/resourcepack/{slug}",
                _ => $"https://modrinth.com/mod/{slug}"
            };
            item.DownloadCount = hit.GetProperty("downloads").GetInt64();
            item.IconUrl = hit.GetProperty("icon_url").GetString() ?? string.Empty;
            item.LastUpdated = DateTime.TryParse(hit.GetProperty("date_modified").GetString(), out var date) ? date : DateTime.MinValue;
            item.Source = ContentSource.Modrinth;

            var categories = hit.GetProperty("categories");
            foreach (var cat in categories.EnumerateArray())
            {
                var catStr = cat.GetString();
                if (!string.IsNullOrEmpty(catStr)) item.Categories.Add(catStr);
            }

            if (hit.TryGetProperty("versions", out var versions) && versions.ValueKind == JsonValueKind.Array)
            {
                foreach (var version in versions.EnumerateArray())
                {
                    var versionStr = version.GetString();
                    if (!string.IsNullOrWhiteSpace(versionStr))
                        item.MinecraftVersions.Add(versionStr);
                }
            }

            foreach (var loader in new[] { ModLoader.Fabric, ModLoader.Forge, ModLoader.NeoForge, ModLoader.Quilt })
            {
                if (item.Categories.Contains(loader.ToString().ToLower()))
                    item.Loaders.Add(loader);
            }

            return item;
        }
        catch { return null; }
    }

    private ContentItem? CreateItemFromProject(JsonElement project)
    {
        try
        {
            var projectType = project.GetProperty("project_type").GetString();
            var type = projectType switch
            {
                "modpack" => ContentType.Modpack,
                "resourcepack" => ContentType.ResourcePack,
                _ => ContentType.Mod
            };

            ContentItem item = type == ContentType.ResourcePack ? new ResourcePackItem() :
                               type == ContentType.Modpack ? new ModpackContentItem() :
                               new ModItem();

            item.Id = project.GetProperty("id").GetString() ?? Guid.NewGuid().ToString();
            item.Title = project.GetProperty("title").GetString() ?? "Unknown";
            item.Description = project.GetProperty("description").GetString() ?? string.Empty;
            item.Author = "Unknown";
            item.PageUrl = $"https://modrinth.com/{projectType}/{project.GetProperty("slug").GetString()}";
            item.DownloadCount = project.GetProperty("downloads").GetInt64();
            item.IconUrl = project.GetProperty("icon_url").GetString() ?? string.Empty;
            item.LastUpdated = project.TryGetProperty("updated", out var updated) && DateTime.TryParse(updated.GetString(), out var updatedDate)
                ? updatedDate
                : project.TryGetProperty("date_modified", out var modified) && DateTime.TryParse(modified.GetString(), out var modifiedDate)
                    ? modifiedDate
                    : DateTime.MinValue;
            item.Source = ContentSource.Modrinth;

            if (project.TryGetProperty("categories", out var categories) && categories.ValueKind == JsonValueKind.Array)
            {
                foreach (var category in categories.EnumerateArray())
                {
                    var value = category.GetString();
                    if (!string.IsNullOrWhiteSpace(value))
                        item.Categories.Add(value);
                }
            }

            if (project.TryGetProperty("game_versions", out var gameVersions) && gameVersions.ValueKind == JsonValueKind.Array)
            {
                foreach (var version in gameVersions.EnumerateArray())
                {
                    var value = version.GetString();
                    if (!string.IsNullOrWhiteSpace(value))
                        item.MinecraftVersions.Add(value);
                }
            }

            if (project.TryGetProperty("loaders", out var loaders) && loaders.ValueKind == JsonValueKind.Array)
            {
                foreach (var loaderValue in loaders.EnumerateArray())
                {
                    if (Enum.TryParse<ModLoader>(loaderValue.GetString(), true, out var loader) && !item.Loaders.Contains(loader))
                        item.Loaders.Add(loader);
                }
            }

            return item;
        }
        catch { return null; }
    }

    public async Task<string?> GetDownloadUrlAsync(string id, string? gameVersion = null, string? loader = null, CancellationToken cancellationToken = default)
    {
        try
        {
            var url = $"{ApiBase}/project/{id}/version";
            var json = await HttpClient.GetStringAsync(url, cancellationToken);
            using var doc = JsonDocument.Parse(json);

            foreach (var version in doc.RootElement.EnumerateArray())
            {
                var versionNumber = version.GetProperty("version_number").GetString() ?? "";

                if (!string.IsNullOrWhiteSpace(gameVersion))
                {
                    var gameVersions = version.GetProperty("game_versions");
                    var matchesVersion = false;
                    foreach (var gv in gameVersions.EnumerateArray())
                    {
                        if (gv.GetString()?.Contains(gameVersion) == true)
                        {
                            matchesVersion = true;
                            break;
                        }
                    }
                    if (!matchesVersion) continue;
                }

                if (!string.IsNullOrWhiteSpace(loader))
                {
                    var loaders = version.GetProperty("loaders");
                    var matchesLoader = false;
                    foreach (var ld in loaders.EnumerateArray())
                    {
                        if (ld.GetString()?.Equals(loader, StringComparison.OrdinalIgnoreCase) == true)
                        {
                            matchesLoader = true;
                            break;
                        }
                    }
                    if (!matchesLoader) continue;
                }

                var files = version.GetProperty("files");
                foreach (var file in files.EnumerateArray())
                {
                    if (file.GetProperty("primary").GetBoolean())
                        return file.GetProperty("url").GetString();
                }

                var firstFile = files.EnumerateArray().FirstOrDefault();
                if (firstFile.ValueKind != JsonValueKind.Undefined)
                    return firstFile.GetProperty("url").GetString();
            }

            return null;
        }
        catch { return null; }
    }
}

public class CurseForgeParser : BaseContentParser
{
    private const string SearchUrl = "https://www.curseforge.com/minecraft/search";
    public override ContentSource Source => ContentSource.CurseForge;

    public CurseForgeParser(HttpClient httpClient) : base(httpClient) { }

    public override async Task<ContentSearchResult> SearchAsync(ContentFilter filter, int page = 1, int pageSize = 20, CancellationToken cancellationToken = default)
    {
        var classParam = filter.ContentType switch
        {
            ContentType.Modpack => "modpacks",
            ContentType.ResourcePack => "texture-packs",
            _ => "mc-mods"
        };

        var url = $"{SearchUrl}?class={classParam}&page={page}&pageSize={pageSize}&sortBy=relevancy&search={Uri.EscapeDataString(filter.SearchQuery ?? "")}";
        using var request = new HttpRequestMessage(HttpMethod.Get, url);
        request.Headers.TryAddWithoutValidation("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/124.0 Safari/537.36");
        request.Headers.TryAddWithoutValidation("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
        request.Headers.TryAddWithoutValidation("Accept-Language", "en-US,en;q=0.9,it-IT;q=0.8,it;q=0.7");
        request.Headers.Referrer = new Uri("https://www.curseforge.com/");

        using var response = await HttpClient.SendAsync(request, cancellationToken);
        if ((int)response.StatusCode == 403)
        {
            // CurseForge can block automated traffic; fail softly and let other sources continue.
            return new ContentSearchResult
            {
                Items = new List<ContentItem>(),
                TotalCount = 0,
                Page = page,
                PageSize = pageSize,
                HasMorePages = false
            };
        }
        response.EnsureSuccessStatusCode();
        var html = await response.Content.ReadAsStringAsync(cancellationToken);
        var doc = new HtmlDocument();
        doc.LoadHtml(html);

        var items = new List<ContentItem>();
        var projectCards = doc.DocumentNode.SelectNodes("//div[contains(@class, 'project-card')]");
        
        if (projectCards == null) return new ContentSearchResult { Items = items, Page = page, PageSize = pageSize };

        foreach (var card in projectCards)
        {
            try
            {
                ContentItem item = filter.ContentType == ContentType.ResourcePack ? new ResourcePackItem() :
                                   filter.ContentType == ContentType.Modpack ? new ModpackContentItem() :
                                   new ModItem();

                var titleNode = card.SelectSingleNode(".//h3[contains(@class, 'text-lg')]");
                if (titleNode != null)
                    item.Title = titleNode.InnerText.Trim();

                var descNode = card.SelectSingleNode(".//p[contains(@class, 'text-sm')]");
                if (descNode != null)
                    item.Description = descNode.InnerText.Trim();

                var linkNode = card.SelectSingleNode(".//a[contains(@href, '/minecraft')]");
                if (linkNode != null)
                {
                    var href = linkNode.GetAttributeValue("href", "");
                    item.PageUrl = $"https://www.curseforge.com{href}";
                    item.Id = href.Split('/').LastOrDefault() ?? Guid.NewGuid().ToString();
                }

                var imgNode = card.SelectSingleNode(".//img");
                if (imgNode != null)
                    item.IconUrl = imgNode.GetAttributeValue("src", "");

                var authorNode = card.SelectSingleNode(".//span[contains(@class, 'author')]");
                if (authorNode != null)
                    item.Author = authorNode.InnerText.Trim();

                var downloadsNode = card.SelectSingleNode(".//span[contains(@class, 'downloads')]");
                if (downloadsNode != null)
                {
                    var dlText = downloadsNode.InnerText.Trim().ToLower();
                    item.DownloadCount = ParseDownloads(dlText);
                }

                item.Source = ContentSource.CurseForge;
                items.Add(item);
            }
            catch { }
        }

        return new ContentSearchResult
        {
            Items = items,
            TotalCount = items.Count,
            Page = page,
            PageSize = pageSize,
            HasMorePages = items.Count == pageSize
        };
    }

    public override async Task<ContentItem?> GetDetailsAsync(string id, CancellationToken cancellationToken = default)
    {
        return null;
    }

    public async Task<string?> GetDownloadUrlAsync(string id, string? gameVersion = null, string? loader = null, CancellationToken cancellationToken = default)
    {
        return null;
    }

    private static long ParseDownloads(string text)
    {
        try
        {
            text = text.Replace(",", "").Replace(" downloads", "").Trim();
            if (text.EndsWith("k"))
                return (long)(double.Parse(text.TrimEnd('k')) * 1000);
            if (text.EndsWith("m"))
                return (long)(double.Parse(text.TrimEnd('m')) * 1000000);
            return long.Parse(text);
        }
        catch { return 0; }
    }
}
