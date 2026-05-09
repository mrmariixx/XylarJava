import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public final class ModrinthIndexSummary {
    private static final Pattern NAME_PATTERN =
        Pattern.compile("\"name\"\\s*:\\s*\"([^\"]+)\"");
    private static final Pattern DEP_PATTERN_TEMPLATE =
        Pattern.compile("\"%s\"\\s*:\\s*(\"([^\"]+)\"|([0-9][^,}\\s]*))");

    private ModrinthIndexSummary() {
    }

    public static void main(String[] args) throws Exception {
        if (args.length == 0) {
            System.out.println("Usage: java ModrinthIndexSummary <pack-root|modrinth.index.json>");
            return;
        }

        Path input = Paths.get(args[0]).toAbsolutePath().normalize();
        Path indexPath = Files.isDirectory(input) ? input.resolve("modrinth.index.json") : input;

        if (!Files.exists(indexPath)) {
            System.err.println("modrinth.index.json not found: " + indexPath);
            System.exit(1);
            return;
        }

        String json = Files.readString(indexPath);
        String packName = extractValue(NAME_PATTERN, json, "Modpack");
        String minecraftVersion = extractDependency(json, "minecraft", "unknown");
        LoaderInfo loaderInfo = detectLoader(json);

        System.out.println("Pack: " + packName);
        System.out.println("Minecraft: " + minecraftVersion);
        System.out.println("Loader: " + loaderInfo.name());
        if (!loaderInfo.version().isBlank()) {
            System.out.println("Loader version: " + loaderInfo.version());
        }
        System.out.println("Index path: " + indexPath);
    }

    private static LoaderInfo detectLoader(String json) {
        String fabric = extractDependency(json, "fabric-loader", "");
        if (!fabric.isBlank()) {
            return new LoaderInfo("Fabric", fabric);
        }

        String forge = extractDependency(json, "forge", "");
        if (!forge.isBlank()) {
            return new LoaderInfo("Forge", forge);
        }

        return new LoaderInfo("Vanilla", "");
    }

    private static String extractDependency(String json, String key, String fallback) {
        Pattern pattern = Pattern.compile(String.format(DEP_PATTERN_TEMPLATE.pattern(), Pattern.quote(key)));
        Matcher matcher = pattern.matcher(json);
        if (!matcher.find()) {
            return fallback;
        }

        String stringValue = matcher.group(2);
        if (stringValue != null && !stringValue.isBlank()) {
            return stringValue;
        }

        String numericValue = matcher.group(3);
        return numericValue != null && !numericValue.isBlank() ? numericValue : fallback;
    }

    private static String extractValue(Pattern pattern, String content, String fallback) {
        Matcher matcher = pattern.matcher(content);
        return matcher.find() ? matcher.group(1) : fallback;
    }

    private record LoaderInfo(String name, String version) {
    }
}
