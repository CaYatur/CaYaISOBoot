using System.ComponentModel;
using System.Diagnostics;
using System.Security.Principal;
using System.Text;
using CaYaISOBoot.Core.Interfaces;

namespace CaYaISOBoot.Disk;

public sealed class SafeEjectService
{
    private readonly IAppLogger _logger;

    public SafeEjectService(IAppLogger logger)
    {
        _logger = logger;
    }

    public int? TryGetDiskNumberForRoot(string rootPath)
    {
        if (string.IsNullOrWhiteSpace(rootPath))
        {
            return null;
        }

        try
        {
            return WindowsStorageDevice.Query(rootPath).DiskNumber;
        }
        catch
        {
            return null;
        }
    }

    public async Task<SafeEjectResult> EjectByDiskNumberAsync(
        int diskNumber,
        CancellationToken cancellationToken = default)
    {
        EnsureNotSystemDisk(diskNumber);

        var roots = FindMountedRootsForDisk(diskNumber);
        if (roots.Count == 0)
        {
            throw new InvalidOperationException("Bu USB için kaldırılabilir bağlı sürücü harfi bulunamadı.");
        }

        await _logger.InfoAsync($"Güvenli çıkar başlıyor. Fiziksel disk: {diskNumber}", cancellationToken)
            .ConfigureAwait(false);

        foreach (var root in roots)
        {
            await _logger.InfoAsync($"mountvol /p uygulanacak: {root}", cancellationToken)
                .ConfigureAwait(false);
        }

        var output = IsCurrentProcessElevated()
            ? await RunMountVolDirectAsync(roots, cancellationToken).ConfigureAwait(false)
            : await RunMountVolElevatedAsync(roots, cancellationToken).ConfigureAwait(false);

        await _logger.InfoAsync($"Güvenli çıkar tamamlandı: {string.Join(", ", roots)}", cancellationToken)
            .ConfigureAwait(false);

        return new SafeEjectResult(diskNumber, roots, output);
    }

    private static IReadOnlyList<string> FindMountedRootsForDisk(int diskNumber)
    {
        var systemRoot = NormalizeRoot(Path.GetPathRoot(Environment.GetFolderPath(Environment.SpecialFolder.Windows)) ?? "C:\\");
        var roots = new List<string>();

        foreach (var drive in DriveInfo.GetDrives())
        {
            if (!drive.IsReady)
            {
                continue;
            }

            var root = NormalizeRoot(drive.Name);
            if (root.Equals(systemRoot, StringComparison.OrdinalIgnoreCase))
            {
                continue;
            }

            var storage = WindowsStorageDevice.Query(root);
            if (storage.DiskNumber == diskNumber)
            {
                roots.Add(root);
            }
        }

        return roots
            .Distinct(StringComparer.OrdinalIgnoreCase)
            .OrderByDescending(root => root, StringComparer.OrdinalIgnoreCase)
            .ToArray();
    }

    private static void EnsureNotSystemDisk(int diskNumber)
    {
        var systemRoot = NormalizeRoot(Path.GetPathRoot(Environment.GetFolderPath(Environment.SpecialFolder.Windows)) ?? "C:\\");
        var systemDiskNumber = WindowsStorageDevice.Query(systemRoot).DiskNumber;

        if (systemDiskNumber == diskNumber)
        {
            throw new InvalidOperationException("Windows sistem diski güvenli çıkar hedefi olamaz.");
        }
    }

    private static async Task<string> RunMountVolDirectAsync(
        IReadOnlyList<string> roots,
        CancellationToken cancellationToken)
    {
        var output = new StringBuilder();

        foreach (var root in roots)
        {
            using var process = new Process();
            process.StartInfo = new ProcessStartInfo
            {
                FileName = Path.Combine(Environment.SystemDirectory, "mountvol.exe"),
                UseShellExecute = false,
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                CreateNoWindow = true
            };
            process.StartInfo.ArgumentList.Add(root);
            process.StartInfo.ArgumentList.Add("/p");

            process.Start();
            var standardOutputTask = process.StandardOutput.ReadToEndAsync(cancellationToken);
            var standardErrorTask = process.StandardError.ReadToEndAsync(cancellationToken);
            await process.WaitForExitAsync(cancellationToken).ConfigureAwait(false);

            output.AppendLine(await standardOutputTask.ConfigureAwait(false));
            output.AppendLine(await standardErrorTask.ConfigureAwait(false));

            if (process.ExitCode != 0)
            {
                throw new InvalidOperationException($"mountvol başarısız oldu: {root}. Çıkış kodu: {process.ExitCode}");
            }
        }

        return output.ToString();
    }

    private static async Task<string> RunMountVolElevatedAsync(
        IReadOnlyList<string> roots,
        CancellationToken cancellationToken)
    {
        var scriptPath = Path.Combine(Path.GetTempPath(), $"cayaisoboot-eject-{Guid.NewGuid():N}.cmd");
        var script = new StringBuilder();
        script.AppendLine("@echo off");
        script.AppendLine("setlocal");

        foreach (var root in roots)
        {
            script.AppendLine($"mountvol {root} /p");
            script.AppendLine("if errorlevel 1 exit /b %errorlevel%");
        }

        script.AppendLine("exit /b 0");
        await File.WriteAllTextAsync(scriptPath, script.ToString(), Encoding.ASCII, cancellationToken)
            .ConfigureAwait(false);

        try
        {
            using var process = new Process();
            process.StartInfo = new ProcessStartInfo
            {
                FileName = "cmd.exe",
                Arguments = $"/c \"\"{scriptPath}\"\"",
                UseShellExecute = true,
                Verb = "runas",
                WindowStyle = ProcessWindowStyle.Hidden,
                WorkingDirectory = Path.GetTempPath()
            };

            try
            {
                process.Start();
            }
            catch (Win32Exception ex) when (ex.NativeErrorCode == 1223)
            {
                throw new OperationCanceledException("Yönetici izni kullanıcı tarafından iptal edildi.", ex, cancellationToken);
            }

            await process.WaitForExitAsync(cancellationToken).ConfigureAwait(false);
            if (process.ExitCode != 0)
            {
                throw new InvalidOperationException($"mountvol yükseltilmiş işlemde başarısız oldu. Çıkış kodu: {process.ExitCode}");
            }

            return "mountvol yükseltilmiş işlemle tamamlandı.";
        }
        finally
        {
            try
            {
                File.Delete(scriptPath);
            }
            catch
            {
            }
        }
    }

    private static bool IsCurrentProcessElevated()
    {
        using var identity = WindowsIdentity.GetCurrent();
        var principal = new WindowsPrincipal(identity);
        return principal.IsInRole(WindowsBuiltInRole.Administrator);
    }

    private static string NormalizeRoot(string root)
    {
        var trimmed = root.Trim();
        return trimmed.EndsWith('\\') ? trimmed : trimmed + "\\";
    }
}
