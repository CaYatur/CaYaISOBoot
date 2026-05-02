using CaYaISOBoot.Core.Interfaces;
using CaYaISOBoot.Core.Models;
using CaYaISOBoot.Core.Shared;
using System.Diagnostics;
using System.Text;

namespace CaYaISOBoot.Disk;

public sealed class UsbInstallService
{
    private readonly IAppLogger _logger;
    private static readonly TimeSpan DiskPartHardTimeout = TimeSpan.FromMinutes(30);

    public UsbInstallService(IAppLogger logger)
    {
        _logger = logger;
    }

    public Task<InstallPlan> CreateSafeInstallPlanAsync(
        UsbDiskInfo targetDisk,
        string payloadRoot,
        CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        if (!targetDisk.Safety.CanInstall)
        {
            throw new InvalidOperationException("Seçilen disk güvenlik denetiminden geçmedi.");
        }

        var sourceBootPayload = Path.Combine(payloadRoot, "EFI", "BOOT", "BOOTX64.EFI");
        var targetBootPayload = Path.Combine(targetDisk.RootPath, "EFI", "BOOT", "BOOTX64.EFI");
        var directories = AppConstants.InstallDirectories
            .Select(path => Path.Combine(targetDisk.RootPath, path.Replace('/', Path.DirectorySeparatorChar)))
            .ToArray();

        return Task.FromResult(new InstallPlan(
            targetDisk.RootPath,
            directories,
            sourceBootPayload,
            targetBootPayload,
            File.Exists(sourceBootPayload)));
    }

    public Task<CleanInstallPlan> CreateCleanInstallPlanAsync(
        UsbDiskInfo targetDisk,
        string payloadRoot,
        CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();
        ValidateCleanInstallTarget(targetDisk);

        var sourceBootPayload = Path.Combine(payloadRoot, "EFI", "BOOT", "BOOTX64.EFI");
        var bootLetter = PickAvailableDriveLetter(preferred: 'X');
        var dataLetter = PickAvailableDriveLetter(excluded: bootLetter, preferred: 'Y');
        // exFAT: FAT32'nin 4 GB tek dosya limitini aşar; modern UEFI firmware tarafından okunabilir
        var dataFileSystem = "exFAT";
        var commands = new[]
        {
            $"select disk {targetDisk.Storage.DiskNumber!.Value}",
            "online disk noerr",
            "attributes disk clear readonly noerr",
            "clean",
            "convert mbr",
            "create partition primary size=512",
            "format fs=fat32 quick label=CAYABOOT override",
            "active",
            $"assign letter={bootLetter}",
            "create partition primary",
            $"format fs={dataFileSystem} quick label=CAYAISO override",
            $"assign letter={dataLetter}",
            "exit"
        };

        return Task.FromResult(new CleanInstallPlan(
            targetDisk.Storage.DiskNumber.Value,
            $"{bootLetter}:",
            $"{dataLetter}:",
            dataFileSystem,
            sourceBootPayload,
            File.Exists(sourceBootPayload),
            commands));
    }

    public async Task<CleanInstallResult> InstallCleanAsync(
        UsbDiskInfo targetDisk,
        string payloadRoot,
        IProgress<InstallProgress>? progress = null,
        CancellationToken cancellationToken = default)
    {
        var plan = await CreateCleanInstallPlanAsync(targetDisk, payloadRoot, cancellationToken).ConfigureAwait(false);
        if (!plan.BootPayloadExists)
        {
            throw new FileNotFoundException(
                "BOOTX64.EFI payload bulunamadı. Boot motoru build sırasında üretilmeli.",
                plan.BootPayloadSource);
        }

        progress?.Report(new InstallProgress(3, "Fiziksel disk son kez doğrulanıyor."));
        ValidateCleanInstallTarget(targetDisk);

        await _logger.WarningAsync($"Temiz kurulum başlıyor. Hedef fiziksel disk: {plan.DiskNumber}", cancellationToken)
            .ConfigureAwait(false);

        progress?.Report(new InstallProgress(10, "USB yeniden bölümlendiriliyor."));
        await VerifyPhysicalDiskBeforeDiskPartAsync(targetDisk, plan.DiskNumber, targetDisk.RootPath, cancellationToken).ConfigureAwait(false);
        var bootRoot = plan.BootDriveLetter + "\\";
        var dataRoot = plan.DataDriveLetter + "\\";
        var diskPartLog = new StringBuilder();

        diskPartLog.AppendLine(await RunDiskPartAsync(
            [
                $"select disk {plan.DiskNumber}",
                "online disk noerr",
                "attributes disk clear readonly noerr",
                "exit"
            ],
            cancellationToken).ConfigureAwait(false));

        progress?.Report(new InstallProgress(18, "USB temizleniyor."));
        await VerifyPhysicalDiskBeforeDiskPartAsync(targetDisk, plan.DiskNumber, targetDisk.RootPath, cancellationToken).ConfigureAwait(false);
        diskPartLog.AppendLine(await RunDiskPartAsync(
            [
                $"select disk {plan.DiskNumber}",
                "clean",
                "convert mbr",
                "exit"
            ],
            cancellationToken).ConfigureAwait(false));

        progress?.Report(new InstallProgress(30, "CAYABOOT bölümü oluşturuluyor ve formatlanıyor."));
        await VerifyPhysicalDiskBeforeDiskPartAsync(targetDisk, plan.DiskNumber, null, cancellationToken).ConfigureAwait(false);
        diskPartLog.AppendLine(await RunDiskPartAsync(
            [
                $"select disk {plan.DiskNumber}",
                "create partition primary size=512",
                "format fs=fat32 quick label=CAYABOOT override",
                "active",
                $"assign letter={plan.BootDriveLetter.TrimEnd(':')}",
                "exit"
            ],
            cancellationToken).ConfigureAwait(false));
        await WaitForDriveAsync(bootRoot, cancellationToken).ConfigureAwait(false);

        progress?.Report(new InstallProgress(43, "CAYAISO bölümü oluşturuluyor ve formatlanıyor."));
        await VerifyPhysicalDiskBeforeDiskPartAsync(targetDisk, plan.DiskNumber, bootRoot, cancellationToken).ConfigureAwait(false);
        diskPartLog.AppendLine(await RunDiskPartAsync(
            [
                $"select disk {plan.DiskNumber}",
                "create partition primary",
                $"format fs={plan.DataFileSystem} quick label=CAYAISO override",
                $"assign letter={plan.DataDriveLetter.TrimEnd(':')}",
                "exit"
            ],
            cancellationToken).ConfigureAwait(false));
        await WaitForDriveAsync(dataRoot, cancellationToken).ConfigureAwait(false);
        await VerifyPhysicalDiskBeforeDiskPartAsync(targetDisk, plan.DiskNumber, dataRoot, cancellationToken).ConfigureAwait(false);

        progress?.Report(new InstallProgress(55, "Boot bölümü hazırlanıyor."));
        Directory.CreateDirectory(Path.Combine(bootRoot, "EFI", "BOOT"));
        Directory.CreateDirectory(Path.Combine(bootRoot, "CaYaISOBoot", "system"));
        Directory.CreateDirectory(Path.Combine(bootRoot, "CaYaISOBoot", "config"));
        Directory.CreateDirectory(Path.Combine(bootRoot, "CaYaISOBoot", "logs"));
        Directory.CreateDirectory(Path.Combine(bootRoot, "CaYaISOBoot", "cache"));
        Directory.CreateDirectory(Path.Combine(bootRoot, "CaYaISOBoot", "drivers"));
        Directory.CreateDirectory(Path.Combine(bootRoot, "CaYaISOBoot", "ISO"));

        var targetBootPayload = Path.Combine(bootRoot, "EFI", "BOOT", "BOOTX64.EFI");
        progress?.Report(new InstallProgress(68, "BOOTX64.EFI boot bölümüne kopyalanıyor."));
        await CopyFileWithProgressAsync(plan.BootPayloadSource, targetBootPayload, progress, cancellationToken)
            .ConfigureAwait(false);

        // exFAT UEFI sürücüsü varsa kopyala (CAYAISO exFAT bölümü firmware'de görünür hale gelir)
        var sourceExFatDriver = Path.Combine(payloadRoot, "CaYaISOBoot", "drivers", "exfat.efi");
        if (File.Exists(sourceExFatDriver))
        {
            var targetExFatDriver = Path.Combine(bootRoot, "CaYaISOBoot", "drivers", "exfat.efi");
            progress?.Report(new InstallProgress(74, "exFAT UEFI sürücüsü kopyalanıyor."));
            await CopyFileWithProgressAsync(sourceExFatDriver, targetExFatDriver, progress, cancellationToken)
                .ConfigureAwait(false);
        }

        await InstallWinPeBridgeAsync(bootRoot, payloadRoot, progress, 78, cancellationToken)
            .ConfigureAwait(false);

        var configText = $"""
            CaYaISOBoot Clean Install
            Time: {DateTimeOffset.Now:O}
            PhysicalDisk: {plan.DiskNumber}
            DataLabel: CAYAISO
            DataFileSystem: {plan.DataFileSystem}
            CAYAISO is left empty for user ISO/IMG/EFI files.
            CaYa system files are stored on the hidden CAYABOOT partition.
            """;
        await File.WriteAllTextAsync(Path.Combine(bootRoot, "CaYaISOBoot", "config", "install.info"), configText, cancellationToken)
            .ConfigureAwait(false);

        progress?.Report(new InstallProgress(88, "Boot bölümü gizleniyor, veri bölümü açık bırakılıyor."));
        await VerifyPhysicalDiskBeforeDiskPartAsync(targetDisk, plan.DiskNumber, dataRoot, cancellationToken).ConfigureAwait(false);
        var bootLetterOnly = plan.BootDriveLetter.TrimEnd(':');
        var removeBootLetterOutput = await RunDiskPartAsync(
            [
                $"select disk {plan.DiskNumber}",
                "select partition 1",
                $"remove letter={bootLetterOnly} noerr",
                "exit"
            ],
            cancellationToken).ConfigureAwait(false);

        await _logger.InfoAsync($"Temiz kurulum tamamlandı. Veri bölümü: {dataRoot}", cancellationToken)
            .ConfigureAwait(false);
        progress?.Report(new InstallProgress(100, "Temiz kurulum tamamlandı."));

        return new CleanInstallResult(
            plan.DiskNumber,
            dataRoot,
            plan.DataFileSystem,
            targetBootPayload,
            diskPartLog.ToString() + Environment.NewLine + removeBootLetterOutput);
    }

    public async Task<BootEngineUpdateResult> UpdateBootEngineAsync(
        UsbDiskInfo targetDisk,
        string payloadRoot,
        IProgress<InstallProgress>? progress = null,
        CancellationToken cancellationToken = default)
    {
        ValidateCleanInstallTarget(targetDisk);

        var sourceBootPayload = Path.Combine(payloadRoot, "EFI", "BOOT", "BOOTX64.EFI");
        if (!File.Exists(sourceBootPayload))
        {
            throw new FileNotFoundException(
                "BOOTX64.EFI payload bulunamadı. Boot motoru build sırasında üretilmeli.",
                sourceBootPayload);
        }

        var targetDiskNumber = targetDisk.Storage.DiskNumber!.Value;
        var selectedDriveLetter = string.IsNullOrWhiteSpace(targetDisk.DriveLetter)
            ? (char?)null
            : targetDisk.DriveLetter[0];
        var bootLetter = PickAvailableDriveLetter(excluded: selectedDriveLetter, preferred: 'X');
        var bootDrive = $"{bootLetter}:";
        var bootRoot = bootDrive + "\\";
        var diskPartOutput = new StringBuilder();
        var mounted = false;
        Exception? primaryException = null;

        await _logger.InfoAsync($"Boot motoru güncellemesi başlıyor. Hedef fiziksel disk: {targetDiskNumber}", cancellationToken)
            .ConfigureAwait(false);

        try
        {
            progress?.Report(new InstallProgress(10, "CAYABOOT bölümü geçici olarak bağlanıyor."));
            await VerifyPhysicalDiskBeforeDiskPartAsync(targetDisk, targetDiskNumber, targetDisk.RootPath, cancellationToken).ConfigureAwait(false);
            diskPartOutput.AppendLine(await RunDiskPartAsync(
                [
                    $"select disk {targetDiskNumber}",
                    "select partition 1",
                    $"assign letter={bootLetter}",
                    "exit"
                ],
                cancellationToken).ConfigureAwait(false));
            mounted = true;

            await WaitForDriveAsync(bootRoot, cancellationToken).ConfigureAwait(false);
            var bootDriveInfo = new DriveInfo(bootRoot);
            if (!bootDriveInfo.IsReady)
            {
                throw new InvalidOperationException("CAYABOOT bölümü hazır hale gelmedi.");
            }

            if (!string.Equals(bootDriveInfo.VolumeLabel, "CAYABOOT", StringComparison.OrdinalIgnoreCase))
            {
                throw new InvalidOperationException(
                    $"Disk {targetDiskNumber} üzerindeki 1. bölüm CAYABOOT olarak doğrulanamadı. Bulunan etiket: {bootDriveInfo.VolumeLabel}");
            }

            if (!string.Equals(bootDriveInfo.DriveFormat, "FAT32", StringComparison.OrdinalIgnoreCase))
            {
                throw new InvalidOperationException(
                    $"CAYABOOT bölümü FAT32 değil. Bulunan dosya sistemi: {bootDriveInfo.DriveFormat}");
            }

            Directory.CreateDirectory(Path.Combine(bootRoot, "EFI", "BOOT"));
            Directory.CreateDirectory(Path.Combine(bootRoot, "CaYaISOBoot", "config"));
            var targetBootPayload = Path.Combine(bootRoot, "EFI", "BOOT", "BOOTX64.EFI");

            progress?.Report(new InstallProgress(55, "Yeni BOOTX64.EFI CAYABOOT bölümüne kopyalanıyor."));
            await CopyFileWithProgressAsync(sourceBootPayload, targetBootPayload, progress, cancellationToken)
                .ConfigureAwait(false);

            await InstallWinPeBridgeAsync(bootRoot, payloadRoot, progress, 78, cancellationToken)
                .ConfigureAwait(false);

            var updateInfo = $"""
                CaYaISOBoot Boot Engine Update
                Time: {DateTimeOffset.Now:O}
                PhysicalDisk: {targetDiskNumber}
                SourcePayload: {sourceBootPayload}
                """;
            await File.WriteAllTextAsync(
                Path.Combine(bootRoot, "CaYaISOBoot", "config", "boot-engine-update.info"),
                updateInfo,
                cancellationToken).ConfigureAwait(false);

            progress?.Report(new InstallProgress(92, "CAYABOOT bölümü tekrar gizleniyor."));
            return new BootEngineUpdateResult(targetDiskNumber, bootRoot, targetBootPayload, diskPartOutput.ToString());
        }
        catch (Exception ex)
        {
            primaryException = ex;
            throw;
        }
        finally
        {
            if (mounted)
            {
                try
                {
                    await VerifyPhysicalDiskBeforeDiskPartAsync(targetDisk, targetDiskNumber, bootRoot, cancellationToken).ConfigureAwait(false);
                    diskPartOutput.AppendLine(await RunDiskPartAsync(
                        [
                            $"select disk {targetDiskNumber}",
                            "select partition 1",
                            $"remove letter={bootLetter} noerr",
                            "exit"
                        ],
                        cancellationToken).ConfigureAwait(false));
                    progress?.Report(new InstallProgress(100, "Boot motoru güncellemesi tamamlandı."));
                }
                catch (Exception removeException)
                {
                    await _logger.WarningAsync($"CAYABOOT bölümü gizlenemedi: {removeException.Message}", cancellationToken)
                        .ConfigureAwait(false);

                    if (primaryException is null)
                    {
                        throw;
                    }
                }
            }
        }
    }

    public async Task<InstallResult> InstallSafeAsync(
        UsbDiskInfo targetDisk,
        string payloadRoot,
        IProgress<InstallProgress>? progress = null,
        CancellationToken cancellationToken = default)
    {
        if (!targetDisk.Safety.CanInstall)
        {
            throw new InvalidOperationException("Seçilen disk güvenlik denetiminden geçmedi.");
        }

        var plan = await CreateSafeInstallPlanAsync(targetDisk, payloadRoot, cancellationToken).ConfigureAwait(false);
        if (!plan.BootPayloadExists)
        {
            throw new FileNotFoundException(
                "BOOTX64.EFI payload bulunamadı. Önce CaYa Native Boot Engine derlenip installer_payload altına kopyalanmalı.",
                plan.BootPayloadSource);
        }

        var root = targetDisk.RootPath;
        var createdDirectories = new List<string>();

        progress?.Report(new InstallProgress(5, "Kurulum hedefi doğrulanıyor."));
        await _logger.InfoAsync($"Güvenli kurulum başladı: {targetDisk.DisplayName}", cancellationToken).ConfigureAwait(false);

        for (var i = 0; i < AppConstants.InstallDirectories.Length; i++)
        {
            cancellationToken.ThrowIfCancellationRequested();
            var directory = Path.Combine(root, AppConstants.InstallDirectories[i].Replace('/', Path.DirectorySeparatorChar));
            Directory.CreateDirectory(directory);
            createdDirectories.Add(directory);

            var percent = 10 + (i + 1) * 35 / AppConstants.InstallDirectories.Length;
            progress?.Report(new InstallProgress(percent, $"Klasör hazırlandı: {AppConstants.InstallDirectories[i]}"));
        }

        var targetBootPayload = Path.Combine(root, "EFI", "BOOT", "BOOTX64.EFI");
        progress?.Report(new InstallProgress(55, "BOOTX64.EFI kopyalanıyor."));
        await CopyFileWithProgressAsync(plan.BootPayloadSource, plan.BootPayloadTarget, progress, cancellationToken).ConfigureAwait(false);

        await InstallWinPeBridgeAsync(root, payloadRoot, progress, 78, cancellationToken).ConfigureAwait(false);

        var summaryPath = Path.Combine(root, "CaYaISOBoot", "logs", "install-summary.txt");
        var summary = $"""
            CaYaISOBoot Safe Install
            Time: {DateTimeOffset.Now:O}
            Target: {targetDisk.DisplayName}
            FileSystem: {targetDisk.FileSystem}
            BusType: {targetDisk.Storage.BusType}
            Mevcut kullanıcı dosyaları korundu.
            """;

        await File.WriteAllTextAsync(summaryPath, summary, cancellationToken).ConfigureAwait(false);
        await _logger.InfoAsync($"Kurulum tamamlandı: {plan.BootPayloadTarget}", cancellationToken).ConfigureAwait(false);
        progress?.Report(new InstallProgress(100, "Kurulum tamamlandı."));

        return new InstallResult(root, createdDirectories, plan.BootPayloadTarget);
    }

    private async Task InstallWinPeBridgeAsync(
        string targetRoot,
        string payloadRoot,
        IProgress<InstallProgress>? progress,
        int progressPercent,
        CancellationToken cancellationToken)
    {
        var sourceBridge = Path.Combine(payloadRoot, "CaYaISOBoot", "winpe", "CaYaWinPeBridge.exe");
        if (!File.Exists(sourceBridge))
        {
            throw new FileNotFoundException(
                "CaYaWinPeBridge.exe payload bulunamadi. Boot payload build islemini tekrar calistirin.",
                sourceBridge);
        }

        var targetBridge = Path.Combine(targetRoot, "CaYaISOBoot", "winpe", "CaYaWinPeBridge.exe");
        var driverRoot = Path.Combine(targetRoot, "CaYaISOBoot", "drivers", "winpe");
        var setupDriverRoot = Path.Combine(targetRoot, "$WinPEDriver$");
        Directory.CreateDirectory(Path.GetDirectoryName(targetBridge)!);
        Directory.CreateDirectory(Path.Combine(targetRoot, "CaYaISOBoot", "cache"));
        Directory.CreateDirectory(driverRoot);
        Directory.CreateDirectory(setupDriverRoot);
        await WriteWinPeDriverReadmeAsync(driverRoot, cancellationToken).ConfigureAwait(false);

        progress?.Report(new InstallProgress(progressPercent, "Windows PE ISO bridge hazirlaniyor."));
        await CopyFileWithProgressAsync(
                sourceBridge,
                targetBridge,
                progress,
                cancellationToken,
                "Windows PE ISO bridge kopyalaniyor.",
                progressPercent,
                8)
            .ConfigureAwait(false);

        var autounattendPath = Path.Combine(targetRoot, "Autounattend.xml");
        if (File.Exists(autounattendPath))
        {
            var existing = await File.ReadAllTextAsync(autounattendPath, cancellationToken).ConfigureAwait(false);
            if (!existing.Contains("CaYaISOBoot WinPE ISO bridge", StringComparison.OrdinalIgnoreCase))
            {
                var backupPath = Path.Combine(targetRoot, "Autounattend.xml.caya-backup");
                if (!File.Exists(backupPath))
                {
                    File.Copy(autounattendPath, backupPath, overwrite: false);
                }
            }
        }

        await File.WriteAllTextAsync(
                autounattendPath,
                BuildWinPeBridgeAutounattendXml(),
                new UTF8Encoding(encoderShouldEmitUTF8Identifier: false),
                cancellationToken)
            .ConfigureAwait(false);
    }

    private static async Task WriteWinPeDriverReadmeAsync(string driverRoot, CancellationToken cancellationToken)
    {
        var readmePath = Path.Combine(driverRoot, "README.txt");
        if (File.Exists(readmePath))
        {
            return;
        }

        const string readme = """
            CaYaISOBoot WinPE Driver Folder

            Put extracted Windows PE storage drivers here as .inf/.sys/.cat files.
            Example use cases: Intel RST/VMD, RAID, NVMe and chipset storage drivers.

            CaYaISOBoot will:
            - load .inf drivers from this folder recursively during WinPE startup,
            - expose this folder to Windows Setup through Autounattend.xml DriverPaths.

            Keep each driver package in its own subfolder.
            Do not put installer .exe files here; extract the driver package first.
            """;

        await File.WriteAllTextAsync(
                readmePath,
                readme,
                new UTF8Encoding(encoderShouldEmitUTF8Identifier: false),
                cancellationToken)
            .ConfigureAwait(false);
    }

    private static string BuildWinPeBridgeAutounattendXml() =>
        """
        <?xml version="1.0" encoding="utf-8"?>
        <unattend xmlns="urn:schemas-microsoft-com:unattend" xmlns:wcm="http://schemas.microsoft.com/WMIConfig/2002/State" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
          <settings pass="windowsPE">
            <component name="Microsoft-Windows-Setup" processorArchitecture="amd64" publicKeyToken="31bf3856ad364e35" language="neutral" versionScope="nonSxS">
              <UseConfigurationSet>true</UseConfigurationSet>
              <UserData>
                <ProductKey>
                  <WillShowUI>Always</WillShowUI>
                </ProductKey>
                <AcceptEula>true</AcceptEula>
              </UserData>
              <RunSynchronous>
                <RunSynchronousCommand wcm:action="add">
                  <Order>1</Order>
                  <Description>CaYaISOBoot WinPE ISO bridge</Description>
                  <Path>cmd.exe /c for %D in (C D E F G H I J K L M N O P Q R S T U V W X Y Z) do if exist %D:\CaYaISOBoot\winpe\CaYaWinPeBridge.exe %D:\CaYaISOBoot\winpe\CaYaWinPeBridge.exe</Path>
                </RunSynchronousCommand>
              </RunSynchronous>
            </component>
            <component name="Microsoft-Windows-PnpCustomizationsWinPE" processorArchitecture="amd64" publicKeyToken="31bf3856ad364e35" language="neutral" versionScope="nonSxS">
              <DriverPaths>
                <PathAndCredentials wcm:action="add" wcm:keyValue="1">
                  <Path>%configsetroot%\CaYaISOBoot\drivers\winpe</Path>
                </PathAndCredentials>
              </DriverPaths>
            </component>
          </settings>
        </unattend>
        """;

    private static async Task CopyFileWithProgressAsync(
        string source,
        string destination,
        IProgress<InstallProgress>? progress,
        CancellationToken cancellationToken,
        string progressMessage = "BOOTX64.EFI kopyalanÄ±yor.",
        int progressBase = 55,
        int progressSpan = 35)
    {
        const int bufferSize = 128 * 1024;
        Directory.CreateDirectory(Path.GetDirectoryName(destination)!);

        await using var sourceStream = new FileStream(source, FileMode.Open, FileAccess.Read, FileShare.Read, bufferSize, useAsync: true);
        await using var destinationStream = new FileStream(destination, FileMode.Create, FileAccess.Write, FileShare.None, bufferSize, useAsync: true);

        var buffer = new byte[bufferSize];
        long copied = 0;
        int read;

        while ((read = await sourceStream.ReadAsync(buffer, cancellationToken).ConfigureAwait(false)) > 0)
        {
            await destinationStream.WriteAsync(buffer.AsMemory(0, read), cancellationToken).ConfigureAwait(false);
            copied += read;

            var percent = sourceStream.Length == 0
                ? progressBase + progressSpan
                : progressBase + (int)Math.Min(progressSpan, copied * progressSpan / sourceStream.Length);

            progress?.Report(new InstallProgress(percent, progressMessage));
        }
    }

    private static void ValidateCleanInstallTarget(UsbDiskInfo targetDisk)
    {
        if (!targetDisk.Safety.CanInstall)
        {
            throw new InvalidOperationException("Seçilen disk güvenlik denetiminden geçmedi.");
        }

        if (targetDisk.Storage.DiskNumber is null)
        {
            throw new InvalidOperationException("Fiziksel disk numarası doğrulanamadı. Temiz kurulum engellendi.");
        }

        if (!targetDisk.IsUsbConnected && !targetDisk.IsRemovable)
        {
            throw new InvalidOperationException("Hedef USB/removable olarak doğrulanamadı. Temiz kurulum engellendi.");
        }

        if (!Directory.Exists(targetDisk.RootPath))
        {
            throw new InvalidOperationException("Seçilen USB artık erişilebilir değil. Diskleri yenileyip tekrar seçin.");
        }

        var currentTargetStorage = WindowsStorageDevice.Query(targetDisk.RootPath);
        if (currentTargetStorage.DiskNumber != targetDisk.Storage.DiskNumber)
        {
            throw new InvalidOperationException("Seçilen USB'nin fiziksel disk numarası değişti. Diskleri yenileyip tekrar seçin.");
        }

        ulong currentTotal = 0;
        try { currentTotal = checked((ulong)new DriveInfo(targetDisk.RootPath).TotalSize); } catch { }
        var currentFingerprint = DiskFingerprint.From(currentTargetStorage, currentTotal);
        if (!targetDisk.Fingerprint.MatchesPhysicalIdentity(currentFingerprint))
        {
            throw new InvalidOperationException(
                "Seçilen USB'nin fiziksel kimliği (serial/model/boyut/bus) değişti. Diskleri yenileyip tekrar seçin.");
        }

        var systemRoot = Path.GetPathRoot(Environment.GetFolderPath(Environment.SpecialFolder.Windows)) ?? "C:\\";
        var systemStorage = WindowsStorageDevice.Query(systemRoot);
        if (systemStorage.DiskNumber == targetDisk.Storage.DiskNumber)
        {
            throw new InvalidOperationException("Hedef fiziksel disk Windows sistem diskiyle aynı görünüyor. İşlem engellendi.");
        }
    }

    private static char PickAvailableDriveLetter(char? excluded = null, char preferred = 'Y')
    {
        var used = DriveInfo.GetDrives()
            .Select(drive => char.ToUpperInvariant(drive.Name[0]))
            .ToHashSet();

        if (excluded is not null)
        {
            used.Add(char.ToUpperInvariant(excluded.Value));
        }

        if (!used.Contains(char.ToUpperInvariant(preferred)))
        {
            return char.ToUpperInvariant(preferred);
        }

        for (var letter = 'Z'; letter >= 'D'; letter--)
        {
            if (!used.Contains(letter))
            {
                return letter;
            }
        }

        throw new InvalidOperationException("Kullanılabilir sürücü harfi bulunamadı.");
    }

    private async Task<string> RunDiskPartAsync(IEnumerable<string> commands, CancellationToken cancellationToken)
    {
        var commandList = commands.ToArray();
        if (commandList.Any(static c => c.Contains('\r') || c.Contains('\n')))
        {
            throw new InvalidOperationException("diskpart komutu içinde satır sonu karakteri var. İşlem güvenlik nedeniyle engellendi.");
        }

        var scriptPath = Path.Combine(Path.GetTempPath(), $"cayaisoboot-diskpart-{Guid.NewGuid():N}.txt");
        await File.WriteAllLinesAsync(scriptPath, commandList, Encoding.ASCII, cancellationToken).ConfigureAwait(false);

        await _logger.InfoAsync($"diskpart çalıştırılıyor ({commandList.Length} komut). Script: {scriptPath}", cancellationToken)
            .ConfigureAwait(false);

        using var timeoutCts = new CancellationTokenSource(DiskPartHardTimeout);
        using var linkedCts = CancellationTokenSource.CreateLinkedTokenSource(cancellationToken, timeoutCts.Token);

        try
        {
            using var process = new Process();
            process.StartInfo = new ProcessStartInfo
            {
                FileName = "diskpart.exe",
                Arguments = $"/s \"{scriptPath}\"",
                UseShellExecute = false,
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                CreateNoWindow = true
            };

            process.Start();
            var outputTask = process.StandardOutput.ReadToEndAsync(linkedCts.Token);
            var errorTask = process.StandardError.ReadToEndAsync(linkedCts.Token);

            try
            {
                await process.WaitForExitAsync(linkedCts.Token).ConfigureAwait(false);
            }
            catch (OperationCanceledException) when (timeoutCts.IsCancellationRequested)
            {
                TryKillProcess(process);
                throw new TimeoutException($"diskpart {DiskPartHardTimeout.TotalMinutes:0} dakika içinde tamamlanmadı. Güvenlik nedeniyle iptal edildi.");
            }
            catch (OperationCanceledException)
            {
                TryKillProcess(process);
                throw;
            }

            var output = await outputTask.ConfigureAwait(false);
            var error = await errorTask.ConfigureAwait(false);
            var combined = output + error;

            if (process.ExitCode != 0)
            {
                throw new InvalidOperationException($"diskpart başarısız oldu. Çıkış kodu: {process.ExitCode}{Environment.NewLine}{combined}");
            }

            return combined;
        }
        finally
        {
            try
            {
                File.Delete(scriptPath);
            }
            catch (Exception cleanupEx)
            {
                await _logger.WarningAsync($"diskpart geçici dosyası silinemedi ({scriptPath}): {cleanupEx.Message}", CancellationToken.None)
                    .ConfigureAwait(false);
            }
        }
    }

    private static void TryKillProcess(Process process)
    {
        try
        {
            if (!process.HasExited)
            {
                process.Kill(entireProcessTree: true);
            }
        }
        catch
        {
        }
    }

    private static async Task WaitForDriveAsync(string rootPath, CancellationToken cancellationToken)
    {
        for (var attempt = 0; attempt < 120; attempt++)
        {
            cancellationToken.ThrowIfCancellationRequested();
            try
            {
                if (Directory.Exists(rootPath))
                {
                    var info = new DriveInfo(rootPath);
                    if (info.IsReady)
                    {
                        return;
                    }
                }
            }
            catch (IOException)
            {
            }

            await Task.Delay(1000, cancellationToken).ConfigureAwait(false);
        }

        throw new IOException($"Beklenen sürücü hazır olmadı: {rootPath}");
    }

    private async Task VerifyPhysicalDiskBeforeDiskPartAsync(
        UsbDiskInfo targetDisk,
        int expectedDiskNumber,
        string? probeRootPath,
        CancellationToken cancellationToken)
    {
        cancellationToken.ThrowIfCancellationRequested();

        if (targetDisk.Storage.DiskNumber is null || targetDisk.Storage.DiskNumber.Value != expectedDiskNumber)
        {
            await FailAsync($"Hedef disk numarası beklenen {expectedDiskNumber} ile uyuşmuyor.", cancellationToken).ConfigureAwait(false);
        }

        if (!targetDisk.IsUsbConnected && !targetDisk.IsRemovable)
        {
            await FailAsync("Hedef disk USB/removable olarak doğrulanamadı.", cancellationToken).ConfigureAwait(false);
        }

        var systemRoot = Path.GetPathRoot(Environment.GetFolderPath(Environment.SpecialFolder.Windows)) ?? "C:\\";
        var systemStorage = WindowsStorageDevice.Query(systemRoot);
        if (systemStorage.DiskNumber == expectedDiskNumber)
        {
            await FailAsync($"Disk {expectedDiskNumber} Windows sistem diskiyle aynı görünüyor.", cancellationToken).ConfigureAwait(false);
        }

        var expectedFingerprint = targetDisk.Fingerprint;

        if (!string.IsNullOrWhiteSpace(probeRootPath) && Directory.Exists(probeRootPath))
        {
            StorageDeviceInfo current;
            try
            {
                current = WindowsStorageDevice.Query(probeRootPath);
            }
            catch (Exception ex)
            {
                await FailAsync($"Hedef sürücü ({probeRootPath}) sorgulanamadı: {ex.Message}", cancellationToken).ConfigureAwait(false);
                return;
            }

            if (current.DiskNumber is null)
            {
                await FailAsync($"Hedef sürücü ({probeRootPath}) için fiziksel disk numarası okunamadı.", cancellationToken).ConfigureAwait(false);
            }
            else if (current.DiskNumber.Value != expectedDiskNumber)
            {
                await FailAsync(
                    $"Hedef sürücünün fiziksel disk numarası değişti (beklenen {expectedDiskNumber}, bulunan {current.DiskNumber}).",
                    cancellationToken).ConfigureAwait(false);
            }

            ulong currentTotal = 0;
            try
            {
                var driveInfo = new DriveInfo(probeRootPath);
                if (driveInfo.IsReady)
                {
                    currentTotal = checked((ulong)driveInfo.TotalSize);
                }
            }
            catch
            {
            }

            var comparisonTotal = currentTotal;
            if (comparisonTotal != 0 &&
                expectedFingerprint.TotalBytes != 0 &&
                comparisonTotal * 4 < expectedFingerprint.TotalBytes)
            {
                // The probe may point at the 512 MB CAYABOOT partition after
                // repartitioning. It is still the same physical disk; do not let
                // partition-visible size alone fail the physical identity check.
                comparisonTotal = expectedFingerprint.TotalBytes;
            }

            var currentFingerprint = DiskFingerprint.From(current, comparisonTotal);
            if (!expectedFingerprint.MatchesPhysicalIdentity(currentFingerprint))
            {
                await FailAsync(
                    $"Disk fingerprint eşleşmedi. Beklenen: [{expectedFingerprint.Describe()}] | Bulunan: [{currentFingerprint.Describe()}].",
                    cancellationToken).ConfigureAwait(false);
            }
        }
        else if (!string.IsNullOrWhiteSpace(probeRootPath))
        {
            await _logger.WarningAsync(
                $"Fingerprint doğrulaması atlandı: probe yolu ({probeRootPath}) erişilebilir değil. Çağıran tarafın bunu beklemesi gerekir.",
                cancellationToken).ConfigureAwait(false);
        }

        await _logger.InfoAsync(
            $"Disk doğrulaması geçti. Beklenen: Disk {expectedDiskNumber}, Serial={expectedFingerprint.SerialNumber ?? "?"}, Boyut={expectedFingerprint.TotalBytes}.",
            cancellationToken).ConfigureAwait(false);
    }

    private async Task FailAsync(string reason, CancellationToken cancellationToken)
    {
        var message = $"Güvenlik nedeniyle işlem iptal edildi: {reason}";
        await _logger.ErrorAsync(message, new InvalidOperationException(reason)).ConfigureAwait(false);
        throw new InvalidOperationException(message);
    }
}
