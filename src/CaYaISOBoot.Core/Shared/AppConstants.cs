namespace CaYaISOBoot.Core.Shared;

public static class AppConstants
{
    public const string ProductName = "CaYaISOBoot";
    public const string ProductSubtitle = "Native Multi-ISO USB Boot System";
    public const string BootEngineName = "CaYa Native Boot Engine";

    public const string BootPayloadRelativePath = "EFI/BOOT/BOOTX64.EFI";

    public static readonly string[] InstallDirectories =
    [
        "EFI/BOOT",
        "CaYaISOBoot/system",
        "CaYaISOBoot/config",
        "CaYaISOBoot/logs",
        "CaYaISOBoot/cache",
        "CaYaISOBoot/drivers",
        "CaYaISOBoot/drivers/winpe",
        "CaYaISOBoot/winpe",
        "$WinPEDriver$",
        "CaYaISOBoot/ISO",
        "ISO"
    ];

    public static readonly string[] BootScanDirectories =
    [
        "",
        "ISO",
        "CaYaISOBoot/ISO"
    ];

    public static readonly string[] SupportedBootExtensions =
    [
        ".iso",
        ".img",
        ".efi"
    ];
}
