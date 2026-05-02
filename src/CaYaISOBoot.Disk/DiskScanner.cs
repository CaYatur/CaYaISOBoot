using CaYaISOBoot.Core.Interfaces;

namespace CaYaISOBoot.Disk;

public sealed class DiskScanner
{
    private readonly DiskSafetyService _safetyService;
    private readonly IAppLogger _logger;

    public DiskScanner(DiskSafetyService safetyService, IAppLogger logger)
    {
        _safetyService = safetyService;
        _logger = logger;
    }

    public Task<IReadOnlyList<UsbDiskInfo>> ScanAsync(CancellationToken cancellationToken = default) =>
        Task.Run(() => ScanCore(cancellationToken), cancellationToken);

    private IReadOnlyList<UsbDiskInfo> ScanCore(CancellationToken cancellationToken)
    {
        var result = new List<UsbDiskInfo>();

        foreach (var drive in DriveInfo.GetDrives())
        {
            cancellationToken.ThrowIfCancellationRequested();

            if (drive.DriveType is DriveType.CDRom or DriveType.Network or DriveType.NoRootDirectory or DriveType.Ram)
            {
                continue;
            }

            try
            {
                if (!drive.IsReady)
                {
                    continue;
                }

                var storage = WindowsStorageDevice.Query(drive.Name);
                var safety = _safetyService.Evaluate(drive, storage);

                if (!safety.CanInstall)
                {
                    _ = _logger.WarningAsync($"{drive.Name} listelenmedi: {string.Join("; ", safety.BlockingReasons)}", cancellationToken);
                    continue;
                }

                result.Add(new UsbDiskInfo(
                    drive.Name,
                    SafeRead(() => drive.VolumeLabel, string.Empty),
                    SafeRead(() => drive.DriveFormat, "Bilinmiyor"),
                    drive.DriveType,
                    checked((ulong)drive.TotalSize),
                    checked((ulong)drive.AvailableFreeSpace),
                    storage,
                    safety));
            }
            catch (Exception ex)
            {
                _ = _logger.WarningAsync($"{drive.Name} okunamadı: {ex.Message}", cancellationToken);
            }
        }

        // Aynı fiziksel diskin (örn. CAYABOOT + CAYAISO) ayrı satırlar olarak
        // görünmesini engelle: DiskNumber'a göre grupla, her gruptan en büyük
        // (kullanıcı veri bölümü) volume'ü temsilci seç. DiskNumber null ise
        // gruplama yapma — dedupe edemiyoruz, ayrı kalsınlar.
        var grouped = result
            .GroupBy(disk => disk.Storage.DiskNumber)
            .SelectMany(group =>
            {
                var partitions = group
                    .OrderBy(d => d.DriveLetter, StringComparer.OrdinalIgnoreCase)
                    .Select(d => new UsbPartitionInfo(d.DriveLetter, d.VolumeLabel, d.FileSystem, d.TotalBytes))
                    .ToArray();
                ulong physicalSum = 0;
                foreach (var p in partitions) { physicalSum += p.TotalBytes; }

                if (group.Key is null)
                {
                    return group.Select(d => d with { Partitions = partitions, PhysicalCapacityBytes = physicalSum });
                }

                var canonical = group
                    .OrderByDescending(disk => disk.TotalBytes)
                    .ThenBy(disk => disk.DriveLetter, StringComparer.OrdinalIgnoreCase)
                    .First();

                return new[]
                {
                    canonical with { Partitions = partitions, PhysicalCapacityBytes = physicalSum }
                };
            });

        return grouped
            .OrderBy(disk => disk.Storage.DiskNumber ?? int.MaxValue)
            .ThenBy(disk => disk.DriveLetter, StringComparer.OrdinalIgnoreCase)
            .ToArray();
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
