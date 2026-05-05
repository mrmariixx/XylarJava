using System;
using System.Collections.Generic;
using System.Net.Http;
using System.Text.Json;
using System.Text.Json.Serialization;
using System.Threading.Tasks;

namespace XylarJavaLauncher
{
    public class ModrinthAPI
    {
        private static readonly HttpClient client = new HttpClient();
        private const string BASE_URL = "https://api.modrinth.com/v2";

        public class ModProject
        {
            [JsonPropertyName("id")]
            public string? Id { get; set; }

            [JsonPropertyName("slug")]
            public string? Slug { get; set; }

            [JsonPropertyName("name")]
            public string? Name { get; set; }

            [JsonPropertyName("description")]
            public string? Description { get; set; }

            [JsonPropertyName("summary")]
            public string? Summary { get; set; }

            [JsonPropertyName("icon_url")]
            public string? IconUrl { get; set; }

            [JsonPropertyName("downloads")]
            public int Downloads { get; set; }

            [JsonPropertyName("followers")]
            public int Followers { get; set; }

            [JsonPropertyName("project_type")]
            public string? ProjectType { get; set; }
        }

        public class ModVersion
        {
            [JsonPropertyName("id")]
            public string? Id { get; set; }

            [JsonPropertyName("version_number")]
            public string? VersionNumber { get; set; }

            [JsonPropertyName("name")]
            public string? Name { get; set; }

            [JsonPropertyName("game_versions")]
            public List<string>? GameVersions { get; set; }

            [JsonPropertyName("loaders")]
            public List<string>? Loaders { get; set; }

            [JsonPropertyName("files")]
            public List<ModFile>? Files { get; set; }
        }

        public class ModFile
        {
            [JsonPropertyName("url")]
            public string? Url { get; set; }

            [JsonPropertyName("filename")]
            public string? Filename { get; set; }

            [JsonPropertyName("primary")]
            public bool IsPrimary { get; set; }
        }

        public class SearchResult
        {
            [JsonPropertyName("hits")]
            public List<ModProject>? Hits { get; set; }

            [JsonPropertyName("total")]
            public int Total { get; set; }
        }

        public static async Task<List<ModProject>?> SearchMods(string query, string type = "mod", int limit = 20, int offset = 0)
        {
            try
            {
                string url = $"{BASE_URL}/search?query={Uri.EscapeDataString(query)}&project_type={type}&limit={limit}&offset={offset}";
                var response = await client.GetAsync(url);
                response.EnsureSuccessStatusCode();

                var json = await response.Content.ReadAsStringAsync();
                var result = JsonSerializer.Deserialize<SearchResult>(json);
                return result?.Hits;
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine($"Modrinth API Search Error: {ex.Message}");
                return null;
            }
        }

        public static async Task<List<ModVersion>?> GetProjectVersions(string projectId, string? gameVersion = null, string? loader = null)
        {
            try
            {
                string url = $"{BASE_URL}/project/{projectId}/versions";
                var response = await client.GetAsync(url);
                response.EnsureSuccessStatusCode();

                var json = await response.Content.ReadAsStringAsync();
                var versions = JsonSerializer.Deserialize<List<ModVersion>>(json);
                
                // Filter by game version and loader if specified
                if (versions != null && (gameVersion != null || loader != null))
                {
                    versions = versions.FindAll(v =>
                    {
                        bool matchesGameVersion = gameVersion == null || (v.GameVersions?.Contains(gameVersion) ?? false);
                        bool matchesLoader = loader == null || (v.Loaders?.Contains(loader) ?? false);
                        return matchesGameVersion && matchesLoader;
                    });
                }

                return versions;
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine($"Modrinth API Versions Error: {ex.Message}");
                return null;
            }
        }

        public static async Task<ModProject?> GetProjectInfo(string projectId)
        {
            try
            {
                string url = $"{BASE_URL}/project/{projectId}";
                var response = await client.GetAsync(url);
                response.EnsureSuccessStatusCode();

                var json = await response.Content.ReadAsStringAsync();
                return JsonSerializer.Deserialize<ModProject>(json);
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine($"Modrinth API Project Error: {ex.Message}");
                return null;
            }
        }
    }
}
