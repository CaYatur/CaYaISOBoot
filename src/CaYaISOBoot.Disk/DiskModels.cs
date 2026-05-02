using System.Text;
using CaYaISOBoot.Core.Shared;

namespace CaYaISOBoot.Disk;

public enum StorageBusType
{
    Unknown = 0,
    Scsi = 1,
    Atapi = 2,
    Ata = 3,
    Ieee1394 = 4,
    Ssa = 5,
    Fibre = 6,
    Usb = 7,
    Raid = 8,
    Iscsi = 9,
    Sas = 10,
    Sata = 11,
    Sd = 12,
    Mmc = 13,
    Virtual = 14,
    FileBackedVirtual = 15,
    Spaces = 16,
    Nvme = 17,
    Scm = 18,
    Ufs = 19
}

public sealed record StorageDeviceInfo(
    StorageBusType BusType,
    bool RemovableMedia,
    int? DiskNumber,
    string? Vendor,
    string? Product,
    string? SerialNumber)
{
    public static StorageDeviceInfo Unknown { get; } =
        new(StorageBusType.Unknown, false, null, null, null, null);

    public bool IsUsbConnected => BusType == StorageBusType.Usb;
}

public sealed record DiskFingerprint(
    int? DiskNumber,
    StorageBusType BusType,
    bool RemovableMedia,
    string? Vendor,
    string? Product,
    string? SerialNumber,
    ulong TotalBytes)
{
    public static DiskFingerprint From(StorageDeviceInfo storage, ulong totalBytes) =>
        new(storage.DiskNumber, storage.BusType, storage.RemovableMedia,
            Normalize(storage.Vendor), Normalize(storage.Product), Normalize(storage.SerialNumber),
            totalBytes);

    public bool MatchesPhysicalIdentity(DiskFingerprint other)
    {
        // Bus + removable: USB diskinin SATA olarak görünmesi anlam değiştirir.
        if (BusType != other.BusType) return false;
        if (RemovableMedia != other.RemovableMedia) return false;

        // Vendor/Product: stable hardware kimliği. Boşsa kıyaslamayı atla.
        if (!StringMatch(Vendor, other.Vendor)) return false;
        if (!StringMatch(Product, other.Product)) return false;

        // Serial: en güçlü kimlik. Her iki tarafta da varsa eşleşmek zorunda.
        if (!StringMatch(SerialNumber, other.SerialNumber)) return false;

        // TotalBytes filesystem-visible size'dır; aynı fiziksel diskte
        // partition layout değişince (clean / repartition) farklı raporlanır.
        // Bu yüzden size'a KATI eşitlik dayatmıyoruz.
        // - Serial + Vendor + Product güçlü eşleşmesi varsa size'ı yok say.
        // - Yoksa, sadece kapasite "sınıfı" değişmediyse kabul et (~%25 tolerans).
        var hasStrongIdentity =
            (!string.IsNullOrWhiteSpace(SerialNumber) && !string.IsNullOrWhiteSpace(other.SerialNumber)) ||
            (!string.IsNullOrWhiteSpace(Product) && !string.IsNullOrWhiteSpace(other.Product));

        if (!hasStrongIdentity && TotalBytes != 0 && other.TotalBytes != 0)
        {
            var max = Math.Max(TotalBytes, other.TotalBytes);
            var min = Math.Min(TotalBytes, other.TotalBytes);
            // %25'ten büyük sapma → güçlü kimlik yok ve farklı boyut: ret.
            if ((max - min) * 4 > max) return false;
        }

        return true;
    }

    public string Describe() =>
        $"Disk {DiskNumber?.ToString() ?? "?"}, Bus={BusType}, Removable={RemovableMedia}, " +
        $"Vendor={Vendor ?? "?"}, Product={Product ?? "?"}, Serial={SerialNumber ?? "?"}, Size={TotalBytes}";

    // Diskpart/INQUIRY string'leri kontrol karakterleri (NUL, vb.) içerebiliyor;
    // ekrana garbled görünmesin ve karşılaştırma kararlı olsun diye temizliyoruz.
    private static string? Normalize(string? value)
    {
        if (string.IsNullOrWhiteSpace(value)) return null;
        var sb = new StringBuilder(value.Length);
        foreach (var c in value)
        {
            if (c == '\0') break;
            if (!char.IsControl(c)) sb.Append(c);
        }
        var result = sb.ToString().Trim();
        return result.Length == 0 ? null : result;
    }

    private static bool StringMatch(string? a, string? b)
    {
        if (string.IsNullOrWhiteSpace(a) || string.IsNullOrWhiteSpace(b)) return true;
        return string.Equals(a.Trim(), b.Trim(), StringComparison.OrdinalIgnoreCase);
    }
}

public sealed record DiskSafetyResult(
    IReadOnlyList<string> BlockingReasons,
    IReadOnlyList<string> Warnings)
{
    public bool CanInstall => BlockingReasons.Count == 0;

    public string Summary
    {
        get
        {
            if (!CanInstall)
            {
                return "Engellendi";
            }

            return Warnings.Count == 0 ? "Güvenli" : "Ek onay gerekli";
        }
    }

    public string DetailText
    {
        get
        {
            if (BlockingReasons.Count > 0)
            {
                return string.Join(Environment.NewLine, BlockingReasons);
            }

            return Warnings.Count > 0 ? string.Join(Environment.NewLine, Warnings) : "Kurulum için uygun.";
        }
    }
}

public sealed record UsbPartitionInfo(
    string DriveLetter,
    string VolumeLabel,
    string FileSystem,
    ulong TotalBytes)
{
    public string CapacityText => ByteFormatter.Format(TotalBytes);

    public string DisplayLine
    {
        get
        {
            var label = string.IsNullOrWhiteSpace(VolumeLabel) ? "Adsız" : VolumeLabel;
            return $"{DriveLetter}  {label}  ·  {FileSystem}  ·  {CapacityText}";
        }
    }
}

public sealed record UsbDiskInfo(
    string RootPath,
    string VolumeLabel,
    string FileSystem,
    DriveType DriveType,
    ulong TotalBytes,
    ulong FreeBytes,
    StorageDeviceInfo Storage,
    DiskSafetyResult Safety)
{
    public IReadOnlyList<UsbPartitionInfo> Partitions { get; init; } = Array.Empty<UsbPartitionInfo>();

    public ulong PhysicalCapacityBytes { get; init; }

    public string DriveLetter => RootPath.TrimEnd('\\');

    public bool IsRemovable => DriveType == DriveType.Removable || Storage.RemovableMedia;

    public bool IsUsbConnected => Storage.IsUsbConnected;

    public string CapacityText => ByteFormatter.Format(TotalBytes);

    public string PhysicalCapacityText => ByteFormatter.Format(PhysicalCapacityBytes == 0 ? TotalBytes : PhysicalCapacityBytes);

    public string FreeSpaceText => ByteFormatter.Format(FreeBytes);

    public DiskFingerprint Fingerprint => DiskFingerprint.From(Storage, TotalBytes);

    public string ModelName
    {
        get
        {
            var vendor = string.IsNullOrWhiteSpace(Storage.Vendor) ? null : Storage.Vendor.Trim();
            var product = string.IsNullOrWhiteSpace(Storage.Product) ? null : Storage.Product.Trim();
            if (vendor is null && product is null)
            {
                return string.IsNullOrWhiteSpace(VolumeLabel) ? "USB cihazı" : VolumeLabel;
            }
            return string.Join(' ', new[] { vendor, product }.Where(s => s is not null));
        }
    }

    public string HeaderText => $"{ModelName}  ·  {PhysicalCapacityText}";

    public IReadOnlyList<UsbPartitionInfo> OtherPartitions =>
        Partitions
            .Where(p => !string.Equals(p.DriveLetter.TrimEnd(':'), DriveLetter.TrimEnd(':'), StringComparison.OrdinalIgnoreCase))
            .ToArray();

    public string DisplayName
    {
        get
        {
            var label = string.IsNullOrWhiteSpace(VolumeLabel) ? "Adsız USB" : VolumeLabel;
            return $"{DriveLetter}  {label}  ({CapacityText})";
        }
    }
}

public sealed record InstallResult(string RootPath, IReadOnlyList<string> CreatedDirectories, string BootPayloadPath);

public sealed record InstallPlan(
    string RootPath,
    IReadOnlyList<string> DirectoriesToCreate,
    string BootPayloadSource,
    string BootPayloadTarget,
    bool BootPayloadExists);

public sealed record CleanInstallPlan(
    int DiskNumber,
    string BootDriveLetter,
    string DataDriveLetter,
    string DataFileSystem,
    string BootPayloadSource,
    bool BootPayloadExists,
    IReadOnlyList<string> DiskPartCommands);

public sealed record CleanInstallResult(
    int DiskNumber,
    string DataDriveRoot,
    string DataFileSystem,
    string BootPayloadPath,
    string DiskPartOutput);

public sealed record BootEngineUpdateResult(
    int DiskNumber,
    string BootDriveRoot,
    string BootPayloadPath,
    string DiskPartOutput);

public sealed record SafeEjectResult(
    int DiskNumber,
    IReadOnlyList<string> RemovedMountPoints,
    string Output);
