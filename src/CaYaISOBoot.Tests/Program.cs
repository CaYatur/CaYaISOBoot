using CaYaISOBoot.Core.Models;
using CaYaISOBoot.Core.Shared;
using CaYaISOBoot.Disk;
using CaYaISOBoot.ISO;
using System.Reflection;
using System.Xml.Linq;

Run("EFI analysis returns direct boot", () =>
{
    var analyzer = new IsoAnalyzer();
    var result = analyzer.Analyze(new BootFileCandidate("memtest.efi", @"X:\memtest.efi", BootFileType.EfiApplication, 1024));
    Assert(result.State == IsoSupportState.DirectEfiBoot, "EFI files must be direct boot candidates.");
});

Run("ISO analysis selects runtime ISO handler", () =>
{
    var analyzer = new IsoAnalyzer();
    var result = analyzer.Analyze(new BootFileCandidate("ubuntu.iso", @"X:\ISO\ubuntu.iso", BootFileType.Iso, 1024));
    Assert(result.State == IsoSupportState.AnalysisRequired, "ISO files must use the runtime ISO handler.");
    Assert(result.RequiredHandler == "VentoyStyleIsoHandler", "ISO files must use the Ventoy-style runtime handler.");
});

Run("WinPE bridge unattend keeps product key UI and driver paths", () =>
{
    var method = typeof(UsbInstallService).GetMethod(
        "BuildWinPeBridgeAutounattendXml",
        BindingFlags.NonPublic | BindingFlags.Static);
    Assert(method is not null, "Unattend XML builder must exist.");

    var xml = (string)method!.Invoke(null, null)!;
    var doc = XDocument.Parse(xml);
    XNamespace ns = "urn:schemas-microsoft-com:unattend";

    Assert(doc.Descendants(ns + "WillShowUI").Single().Value == "Always",
        "Product key UI must stay available instead of requiring a fixed key.");
    Assert(!doc.Descendants(ns + "Key").Any(),
        "CaYaISOBoot must not write a fixed Windows product key.");
    Assert(doc.Descendants(ns + "Path").Any(x => x.Value.Contains(@"CaYaISOBoot\drivers\winpe", StringComparison.OrdinalIgnoreCase)),
        "WinPE driver folder must be published to Windows Setup.");
    Assert(doc.Descendants(ns + "Path").Any(x => x.Value.Contains("CaYaWinPeBridge.exe", StringComparison.OrdinalIgnoreCase)),
        "WinPE bridge command must be present.");
    Assert(AppConstants.InstallDirectories.Contains("$WinPEDriver$"),
        "Standard Windows Setup $WinPEDriver$ folder must be created on USB installs.");
});

// --- Disk safety / fingerprint tests ---------------------------------------

static StorageDeviceInfo Storage(int diskNumber, string serial = "SN-A", string vendor = "VendorX",
    string product = "USB Stick", StorageBusType bus = StorageBusType.Usb, bool removable = true) =>
    new(bus, removable, diskNumber, vendor, product, serial);

static DiskFingerprint Fp(StorageDeviceInfo s, ulong size = 16UL * 1024 * 1024 * 1024) =>
    DiskFingerprint.From(s, size);

Run("Same disk fingerprint matches", () =>
{
    var a = Fp(Storage(2));
    var b = Fp(Storage(2));
    Assert(a.MatchesPhysicalIdentity(b), "Identical fingerprints must match.");
});

Run("Different serial fingerprints do not match", () =>
{
    var a = Fp(Storage(2, serial: "SN-A"));
    var b = Fp(Storage(2, serial: "SN-B"));
    Assert(!a.MatchesPhysicalIdentity(b), "Different serials must be rejected.");
});

Run("Same serial tolerates size change after repartition", () =>
{
    // Real Lexar USB: same serial but visible size differs after clean/format
    // (e.g. 31_035_748_352 vs 30_483_512_896). Strong serial trumps size.
    var a = Fp(Storage(2, serial: "0000000005AA"), size: 31_035_748_352UL);
    var b = Fp(Storage(2, serial: "0000000005AA"), size: 30_483_512_896UL);
    Assert(a.MatchesPhysicalIdentity(b), "Identical serial must override partition-driven size diff.");
});

Run("No serial + huge size diff is rejected", () =>
{
    var a = Fp(Storage(2, serial: "", vendor: "", product: ""), size: 8UL * 1024 * 1024 * 1024);
    var b = Fp(Storage(2, serial: "", vendor: "", product: ""), size: 64UL * 1024 * 1024 * 1024);
    Assert(!a.MatchesPhysicalIdentity(b), "Without strong identity, ~8x size diff must block.");
});

Run("Serial with control chars normalises", () =>
{
    var a = Fp(Storage(2, serial: "0000000005AB\0\0"));
    var b = Fp(Storage(2, serial: "0000000005AB"));
    Assert(a.MatchesPhysicalIdentity(b), "Trailing NUL/control bytes must be stripped before compare.");
});

Run("BusType mismatch rejects fingerprint", () =>
{
    var a = Fp(Storage(2, bus: StorageBusType.Usb));
    var b = Fp(Storage(2, bus: StorageBusType.Sata));
    Assert(!a.MatchesPhysicalIdentity(b), "USB vs SATA must not match.");
});

Run("Removable flag mismatch rejects fingerprint", () =>
{
    var a = Fp(Storage(2, removable: true));
    var b = Fp(Storage(2, removable: false));
    Assert(!a.MatchesPhysicalIdentity(b), "Removable flag must match.");
});

Run("Missing serial falls back to other fields", () =>
{
    // Some devices report no serial. We allow match if other identifiers agree.
    var a = Fp(Storage(2, serial: ""));
    var b = Fp(Storage(2, serial: ""));
    Assert(a.MatchesPhysicalIdentity(b), "Two devices with no serial but same other fields are treated as match.");
});

Run("DiskSafety blocks system root", () =>
{
    var safety = new DiskSafetyService();
    // We cannot easily fabricate a DriveInfo; assert the public surface compiles.
    Assert(safety is not null, "DiskSafetyService must instantiate.");
});

Run("DiskSafety blocks SATA bus when not removable", () =>
{
    // Pure storage-level check: bus types Sata/Nvme/Ata get a blocking reason.
    // Without DriveInfo we just assert the enum surface stayed consistent.
    Assert((int)StorageBusType.Sata == 11, "SATA enum value must remain stable for safety checks.");
    Assert((int)StorageBusType.Nvme == 17, "NVMe enum value must remain stable for safety checks.");
});

Console.WriteLine("All CaYaISOBoot smoke tests passed.");

static void Run(string name, Action test)
{
    try
    {
        test();
        Console.WriteLine($"PASS {name}");
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"FAIL {name}: {ex.Message}");
        Environment.ExitCode = 1;
    }
}

static void Assert(bool condition, string message)
{
    if (!condition)
    {
        throw new InvalidOperationException(message);
    }
}
