using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
using System.Threading;
using System.Threading.Tasks;

namespace XylarJavaLauncher;

public class ContentService
{
    private readonly List<IContentParser> _parsers;
    private readonly ContentMerger _merger;

    public ContentService(HttpClient httpClient)
    {
        _parsers = new List<IContentParser>
        {
            new ModrinthParser(httpClient),
            new CurseForgeParser(httpClient)
        };
        _merger = new ContentMerger();
    }

    public async Task<ContentSearchResult> SearchAsync(ContentFilter filter, int page = 1, int pageSize = 20, CancellationToken cancellationToken = default)
    {
        var allResults = new List<ContentItem>();
        var tasks = new List<Task<ContentSearchResult>>();
        var fetchPageSize = Math.Clamp(Math.Max(pageSize * 2, 60), 20, 100);

        foreach (var parser in _parsers)
        {
            if (!filter.SelectedSources.Any() || filter.SelectedSources.Contains(parser.Source))
            {
                tasks.Add(SearchWithFallbackAsync(parser, filter, page, fetchPageSize, cancellationToken));
            }
        }

        var results = await Task.WhenAll(tasks);

        foreach (var result in results)
        {
            allResults.AddRange(result.Items);
        }

        var merged = _merger.MergeAndDeduplicate(allResults);
        var sorted = ApplySorting(merged, filter.SortBy);
        var curated = FilterReliable(sorted, filter.ContentType, !string.IsNullOrWhiteSpace(filter.SearchQuery));
        var pageItems = curated.Count > 0 ? curated : sorted;
        var hasMorePages = results.Any(r => r.HasMorePages) || pageItems.Count >= fetchPageSize;

        return new ContentSearchResult
        {
            Items = pageItems,
            TotalCount = pageItems.Count,
            Page = page,
            PageSize = pageSize,
            HasMorePages = hasMorePages
        };
    }

    private static async Task<ContentSearchResult> SearchWithFallbackAsync(IContentParser parser, ContentFilter filter, int page, int pageSize, CancellationToken cancellationToken)
    {
        try
        {
            return await parser.SearchAsync(filter, page, pageSize, cancellationToken);
        }
        catch
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
    }

    public async Task<ContentItem?> GetDetailsAsync(string id, ContentSource source, CancellationToken cancellationToken = default)
    {
        var parser = _parsers.FirstOrDefault(p => p.Source == source);
        return parser != null ? await parser.GetDetailsAsync(id, cancellationToken) : null;
    }

    public async Task<string?> GetDownloadUrlAsync(string id, ContentSource source, string? gameVersion = null, string? loader = null, CancellationToken cancellationToken = default)
    {
        var parser = _parsers.FirstOrDefault(p => p.Source == source);
        if (parser == null) return null;

        try
        {
            if (parser is ModrinthParser modrinthParser)
                return await modrinthParser.GetDownloadUrlAsync(id, gameVersion, loader, cancellationToken);
            if (parser is CurseForgeParser curseParser)
                return await curseParser.GetDownloadUrlAsync(id, gameVersion, loader, cancellationToken);
        }
        catch
        {
            return null;
        }
        return null;
    }

    private static List<ContentItem> ApplySorting(List<ContentItem> items, SortOption sortBy)
    {
        return sortBy switch
        {
            SortOption.Popularity => items.OrderByDescending(i => i.DownloadCount).ToList(),
            SortOption.Latest => items.OrderByDescending(i => i.LastUpdated).ToList(),
            SortOption.Name => items.OrderBy(i => i.Title).ToList(),
            _ => items.OrderByDescending(i => i.DownloadCount).ToList()
        };
    }

    private static List<ContentItem> FilterReliable(IEnumerable<ContentItem> items, ContentType contentType, bool hasSearchQuery)
    {
        return items
            .Where(i => !string.IsNullOrWhiteSpace(i.Title))
            .Where(i => !string.IsNullOrWhiteSpace(i.PageUrl))
            .Where(i => i.DownloadCount >= 0)
            .Where(i => hasSearchQuery || i.LastUpdated == DateTime.MinValue || i.LastUpdated >= DateTime.UtcNow.AddYears(-8))
            .ToList();
    }
}

public class ContentMerger
{
    public List<ContentItem> MergeAndDeduplicate(List<ContentItem> items)
    {
        var result = new List<ContentItem>();
        var processed = new HashSet<string>();

        var grouped = items.GroupBy(i => i.GetNormalizedTitle()).ToList();

        foreach (var group in grouped)
        {
            var ordered = group.OrderBy(i => i.Source == ContentSource.Modrinth ? 0 : 1).ToList();
            var primary = ordered.First();

            foreach (var duplicate in ordered.Skip(1))
            {
                MergeMetadata(primary, duplicate);
            }

            if (!processed.Contains(primary.Id))
            {
                result.Add(primary);
                processed.Add(primary.Id);
            }
        }

        return result;
    }

    private static void MergeMetadata(ContentItem primary, ContentItem duplicate)
    {
        if (string.IsNullOrWhiteSpace(primary.Description) && !string.IsNullOrWhiteSpace(duplicate.Description))
            primary.Description = duplicate.Description;
        
        if (string.IsNullOrWhiteSpace(primary.IconUrl) && !string.IsNullOrWhiteSpace(duplicate.IconUrl))
            primary.IconUrl = duplicate.IconUrl;
        
        if (string.IsNullOrWhiteSpace(primary.Author) || primary.Author == "Unknown")
            primary.Author = duplicate.Author;
        
        primary.DownloadCount = Math.Max(primary.DownloadCount, duplicate.DownloadCount);
        
        if (duplicate.LastUpdated > primary.LastUpdated)
            primary.LastUpdated = duplicate.LastUpdated;

        foreach (var loader in duplicate.Loaders.Where(l => !primary.Loaders.Contains(l)))
            primary.Loaders.Add(loader);

        foreach (var cat in duplicate.Categories.Where(c => !primary.Categories.Contains(c)))
            primary.Categories.Add(cat);
    }
}
