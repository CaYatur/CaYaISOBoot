namespace CaYaISOBoot.Disk;

public sealed class DiskSafetyService
{
    private const ulong LargeUsbWarningBytes = 128UL * 1024 * 1024 * 1024;

    public DiskSafetyResult Evaluate(DriveInfo drive, StorageDeviceInfo storage)
    {
        var blocking = new List<string>();
        var warnings = new List<string>();
        var systemRoot = NormalizeRoot(Path.GetPathRoot(Environment.GetFolderPath(Environment.SpecialFolder.Windows)) ?? "C:\\");
        var root = NormalizeRoot(drive.Name);

        if (!drive.IsReady)
        {
            blocking.Add("Sürücü hazır değil.");
            return new DiskSafetyResult(blocking, warnings);
        }

        if (root.Equals(systemRoot, StringComparison.OrdinalIgnoreCase))
        {
            blocking.Add("Windows'un kurulu olduğu sistem sürücüsü hedef olamaz.");
        }

        if (root.StartsWith("C:\\", StringComparison.OrdinalIgnoreCase))
        {
            blocking.Add("C: sürücüsü hedef olamaz.");
        }

        var removableLike = drive.DriveType == DriveType.Removable || storage.RemovableMedia;
        var usbLike = storage.IsUsbConnected;

        if (!removableLike && !usbLike)
        {
            blocking.Add("Sürücü USB/removable olarak doğrulanamadı.");
        }

        if (drive.DriveType == DriveType.Fixed && !usbLike)
        {
            blocking.Add("Dahili veya sabit disk gibi görünen sürücüler engellenir.");
        }

        if (storage.BusType is StorageBusType.Sata or StorageBusType.Nvme or StorageBusType.Ata)
        {
            blocking.Add($"BusType {storage.BusType} dahili disk riski taşır.");
        }

        if (storage.BusType == StorageBusType.Unknown && removableLike)
        {
            warnings.Add("BusType okunamadı; sürücü removable olduğu için sınırlı güvenle listelendi.");
        }

        if (TryGetTotalSize(drive, out var totalBytes) && totalBytes >= LargeUsbWarningBytes)
        {
            warnings.Add("Kapasite yüksek. Harici USB HDD/SSD olabilir; doğru diski seçtiğinizi kontrol edin.");
        }

        var fileSystem = SafeRead(() => drive.DriveFormat, "Bilinmiyor");
        if (!IsFirmwareFriendlyFileSystem(fileSystem))
        {
            warnings.Add("Dosya sistemi her UEFI firmware tarafından boot edilemeyebilir. FAT32 en uyumlu seçenektir.");
        }

        return new DiskSafetyResult(blocking, warnings);
    }

    private static bool TryGetTotalSize(DriveInfo drive, out ulong totalBytes)
    {
        try
        {
            totalBytes = checked((ulong)drive.TotalSize);
            return true;
        }
        catch
        {
            totalBytes = 0;
            return false;
        }
    }

    private static bool IsFirmwareFriendlyFileSystem(string fileSystem) =>
        fileSystem.Equals("FAT32", StringComparison.OrdinalIgnoreCase) ||
        fileSystem.Equals("FAT", StringComparison.OrdinalIgnoreCase);

    private static string NormalizeRoot(string root)
    {
        var trimmed = root.Trim();
        return trimmed.EndsWith('\\') ? trimmed : trimmed + "\\";
    }

    private static T SafeRead<T>(Func<T> read, T fallback)
    {
        try
        {
            return read();
        }
        catch
        {
            return fallback;
        }
    }
}
