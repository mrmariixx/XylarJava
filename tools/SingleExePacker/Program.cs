using System.Diagnostics;
using System.IO.Compression;
using System.Reflection;
using System.Security.Cryptography;

internal static class Program
{
    [STAThread]
    private static int Main(string[] args)
    {
        try
        {
            var baseDir = Path.Combine(
                Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),
                "XylarJava",
                "app");
            Directory.CreateDirectory(baseDir);

            var marker = Path.Combine(baseDir, ".payload.sha256");
            var exePath = Path.Combine(baseDir, "XylarJava.exe");
            var payloadHash = ComputeEmbeddedHash();

            var needsExtract = !File.Exists(exePath)
                || !File.Exists(marker)
                || !string.Equals(File.ReadAllText(marker).Trim(), payloadHash, StringComparison.OrdinalIgnoreCase);

            if (needsExtract)
            {
                ExtractPayload(baseDir);
                File.WriteAllText(marker, payloadHash);
            }

            var psi = new ProcessStartInfo
            {
                FileName = exePath,
                WorkingDirectory = baseDir,
                UseShellExecute = false,
            };
            foreach (var a in args)
                psi.ArgumentList.Add(a);

            using var proc = Process.Start(psi);
            if (proc is null)
                return 1;
            proc.WaitForExit();
            return proc.ExitCode;
        }
        catch (Exception ex)
        {
            try
            {
                System.Windows.Forms.MessageBox.Show(
                    "XylarJava failed to start:\n" + ex.Message,
                    "XylarJava",
                    System.Windows.Forms.MessageBoxButtons.OK,
                    System.Windows.Forms.MessageBoxIcon.Error);
            }
            catch
            {
                Console.Error.WriteLine(ex);
            }
            return 1;
        }
    }

    private static string ComputeEmbeddedHash()
    {
        using var stream = OpenPayloadStream();
        using var sha = SHA256.Create();
        return Convert.ToHexString(sha.ComputeHash(stream));
    }

    private static Stream OpenPayloadStream()
    {
        var asm = Assembly.GetExecutingAssembly();
        var name = asm.GetManifestResourceNames().First(n => n.EndsWith("Payload.zip", StringComparison.OrdinalIgnoreCase));
        return asm.GetManifestResourceStream(name) ?? throw new InvalidOperationException("Embedded payload missing.");
    }

    private static void ExtractPayload(string baseDir)
    {
        var temp = baseDir + ".extracting";
        if (Directory.Exists(temp))
            Directory.Delete(temp, true);
        Directory.CreateDirectory(temp);

        using (var stream = OpenPayloadStream())
        using (var zip = new ZipArchive(stream, ZipArchiveMode.Read))
        {
            zip.ExtractToDirectory(temp, overwriteFiles: true);
        }

        // Replace atomically-ish: delete old files then move
        if (Directory.Exists(baseDir))
        {
            foreach (var path in Directory.GetFileSystemEntries(baseDir))
            {
                try
                {
                    if (Directory.Exists(path)) Directory.Delete(path, true);
                    else File.Delete(path);
                }
                catch { /* locked files may remain until next run */ }
            }
        }
        else
        {
            Directory.CreateDirectory(baseDir);
        }

        foreach (var entry in Directory.GetFileSystemEntries(temp))
        {
            var name = Path.GetFileName(entry);
            var dest = Path.Combine(baseDir, name);
            if (Directory.Exists(entry))
            {
                if (Directory.Exists(dest)) Directory.Delete(dest, true);
                Directory.Move(entry, dest);
            }
            else
            {
                if (File.Exists(dest)) File.Delete(dest);
                File.Move(entry, dest);
            }
        }
        Directory.Delete(temp, true);
    }
}
