using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.Security.Principal;
using System.Windows;
using CaYaISOBoot.App.Services;
using CaYaISOBoot.Core.Interfaces;
using CaYaISOBoot.Core.Models;
using CaYaISOBoot.Disk;

namespace CaYaISOBoot.App.ViewModels;

public sealed class MainViewModel : ObservableObject
{

    private readonly DiskScanner _diskScanner;
    private readonly UsbInstallService _installService;
    private readonly SafeEjectService _safeEjectService;
    private readonly IUserDialogService _dialogService;
    private readonly IAppLogger _logger;
    private readonly string _payloadRoot;
    private readonly string _logDirectory;

    private UsbDiskInfo? _selectedDisk;
    private string _statusText = "Hazır";
    private string _installPrerequisiteText = "Kurulum için önce bir USB disk seçin.";
    private string? _lastDataRoot;
    private bool _payloadReady;
    private double _progressValue;
    private bool _isBusy;

    public MainViewModel(
        DiskScanner diskScanner,
        UsbInstallService installService,
        SafeEjectService safeEjectService,
        IUserDialogService dialogService,
        IAppLogger logger,
        string payloadRoot,
        string logDirectory)
    {
        _diskScanner = diskScanner;
        _installService = installService;
        _safeEjectService = safeEjectService;
        _dialogService = dialogService;
        _logger = logger;
        _payloadRoot = payloadRoot;
        _logDirectory = logDirectory;

        RefreshCommand = new AsyncRelayCommand(RefreshAsync, () => !IsBusy);
        DryRunCommand = new AsyncRelayCommand(DryRunAsync, () => !IsBusy && SelectedDisk?.Safety.CanInstall == true);
        InstallCommand = new AsyncRelayCommand(InstallAsync, () => !IsBusy && SelectedDisk?.Safety.CanInstall == true && PayloadReady);
        CleanInstallCommand = new AsyncRelayCommand(CleanInstallAsync, () => !IsBusy && SelectedDisk?.Safety.CanInstall == true && PayloadReady);
        UpdateBootEngineCommand = new AsyncRelayCommand(UpdateBootEngineAsync, () => !IsBusy && SelectedDisk is { Safety.CanInstall: true, Storage.DiskNumber: not null } && PayloadReady);
        OpenIsoFolderCommand = new RelayCommand(OpenIsoFolder, () => SelectedDisk is not null || _lastDataRoot is not null);
        OpenLogsCommand = new RelayCommand(() => ShellService.OpenFolder(_logDirectory));
        SafeEjectCommand = new AsyncRelayCommand(SafeEjectAsync, () => !IsBusy && (SelectedDisk is not null || _lastDataRoot is not null));

        _logger.MessageWritten += OnLogMessageWritten;
    }

    public ObservableCollection<UsbDiskInfo> Disks { get; } = [];

    public ObservableCollection<string> LogLines { get; } = [];

    public UsbDiskInfo? SelectedDisk
    {
        get => _selectedDisk;
        set
        {
            if (SetProperty(ref _selectedDisk, value))
            {
                RaiseCommandStates();
                RefreshInstallPrerequisiteText();
            }
        }
    }

    public string StatusText
    {
        get => _statusText;
        set => SetProperty(ref _statusText, value);
    }

    public string InstallPrerequisiteText
    {
        get => _installPrerequisiteText;
        set => SetProperty(ref _installPrerequisiteText, value);
    }

    public bool PayloadReady
    {
        get => _payloadReady;
        set
        {
            if (SetProperty(ref _payloadReady, value))
            {
                RaiseCommandStates();
            }
        }
    }

    public double ProgressValue
    {
        get => _progressValue;
        set => SetProperty(ref _progressValue, value);
    }

    public bool IsBusy
    {
        get => _isBusy;
        set
        {
            if (SetProperty(ref _isBusy, value))
            {
                RaiseCommandStates();
            }
        }
    }

    public AsyncRelayCommand RefreshCommand { get; }

    public AsyncRelayCommand DryRunCommand { get; }

    public AsyncRelayCommand InstallCommand { get; }

    public AsyncRelayCommand CleanInstallCommand { get; }

    public AsyncRelayCommand UpdateBootEngineCommand { get; }

    public RelayCommand OpenIsoFolderCommand { get; }

    public RelayCommand OpenLogsCommand { get; }

    public AsyncRelayCommand SafeEjectCommand { get; }

    public Task InitializeAsync() => RefreshAsync();

    private async Task RefreshAsync()
    {
        try
        {
            IsBusy = true;
            ProgressValue = 0;
            RefreshPayloadState();
            StatusText = "Diskler taranıyor";
            await _logger.InfoAsync("USB disk taraması başladı.");

            var disks = await _diskScanner.ScanAsync();
            Disks.Clear();
            foreach (var disk in disks)
            {
                Disks.Add(disk);
            }

            SelectedDisk = null;
            RefreshInstallPrerequisiteText();
            StatusText = disks.Count == 0 ? "Uygun USB disk bulunamadı" : $"{disks.Count} USB disk bulundu";
            await _logger.InfoAsync($"{disks.Count} uygun USB disk listelendi.");
        }
        catch (Exception ex)
        {
            StatusText = "Disk tarama hatası";
            await _logger.ErrorAsync("Disk tarama sırasında hata oluştu.", ex);
            _dialogService.ShowError("Disk tarama hatası", ex.Message);
        }
        finally
        {
            IsBusy = false;
            ProgressValue = 0;
        }
    }

    private async Task InstallAsync()
    {
        if (SelectedDisk is null)
        {
            return;
        }

        if (!ValidatePayloadReady())
        {
            return;
        }

        if (!_dialogService.ConfirmSafeInstallIntro(SelectedDisk))
        {
            await _logger.InfoAsync("Kullanıcı ilk onay ekranında kurulumu iptal etti.");
            return;
        }

        if (!_dialogService.ConfirmSafeInstallFinal(SelectedDisk))
        {
            await _logger.InfoAsync("Kullanıcı son onay ekranında kurulumu iptal etti.");
            return;
        }

        try
        {
            IsBusy = true;
            StatusText = "Güvenli kurulum sürüyor";
            var progress = CreateProgress();

            var result = await _installService.InstallSafeAsync(SelectedDisk, _payloadRoot, progress);
            StatusText = "Güvenli kurulum tamamlandı";
            _dialogService.ShowInfo("Kurulum tamamlandı", $"CaYaISOBoot kuruldu:\n{result.BootPayloadPath}");
        }
        catch (Exception ex)
        {
            StatusText = "Kurulum hatası";
            await _logger.ErrorAsync("Kurulum başarısız oldu.", ex);
            _dialogService.ShowError("Kurulum hatası", ex.Message);
        }
        finally
        {
            IsBusy = false;
            RaiseCommandStates();
        }
    }

    private async Task CleanInstallAsync()
    {
        if (SelectedDisk is null)
        {
            return;
        }

        if (!ValidatePayloadReady())
        {
            return;
        }

        if (!EnsureAdministratorForCleanInstall())
        {
            return;
        }

        CleanInstallPlan plan;
        try
        {
            plan = await _installService.CreateCleanInstallPlanAsync(SelectedDisk, _payloadRoot);
        }
        catch (Exception ex)
        {
            await _logger.ErrorAsync("Temiz kurulum planı oluşturulamadı.", ex);
            _dialogService.ShowError("Temiz kurulum engellendi", ex.Message);
            return;
        }

        if (!_dialogService.ConfirmCleanInstallIntro(SelectedDisk, plan))
        {
            await _logger.InfoAsync("Kullanıcı temiz kurulum ilk onay ekranında işlemi iptal etti.");
            return;
        }

        if (!_dialogService.ConfirmCleanInstallFinal(SelectedDisk, plan))
        {
            await _logger.InfoAsync("Kullanıcı son onay ekranında temiz kurulumu iptal etti.");
            return;
        }

        try
        {
            IsBusy = true;
            StatusText = "USB yapılandırılıyor";
            var progress = CreateProgress();

            var result = await _installService.InstallCleanAsync(SelectedDisk, _payloadRoot, progress);
            _lastDataRoot = result.DataDriveRoot;
            StatusText = "USB hazır";
            await _logger.InfoAsync($"Veri bölümü hazır: {result.DataDriveRoot}");
            _dialogService.ShowInfo(
                "USB hazır",
                $"CaYaISOBoot temiz kurulum tamamlandı.\n\nBoot sistemi ayrı CAYABOOT bölümüne kuruldu.\nKullanıcı dosya alanı: {result.DataDriveRoot}\nDosya sistemi: {result.DataFileSystem}");

            ShellService.OpenFolder(result.DataDriveRoot);
            await RefreshAsync();
        }
        catch (Exception ex)
        {
            StatusText = "Temiz kurulum hatası";
            await _logger.ErrorAsync("Temiz kurulum başarısız oldu.", ex);
            _dialogService.ShowError("Temiz kurulum hatası", ex.Message);
        }
        finally
        {
            IsBusy = false;
            RaiseCommandStates();
        }
    }

    private async Task UpdateBootEngineAsync()
    {
        if (SelectedDisk is null)
        {
            return;
        }

        if (!ValidatePayloadReady())
        {
            return;
        }

        if (!EnsureAdministratorForBootEngineUpdate())
        {
            return;
        }

        if (!_dialogService.ConfirmBootEngineUpdate(SelectedDisk))
        {
            await _logger.InfoAsync("Kullanıcı boot motoru güncellemesini iptal etti.");
            return;
        }

        try
        {
            IsBusy = true;
            StatusText = "Boot motoru güncelleniyor";
            var progress = CreateProgress();

            var result = await _installService.UpdateBootEngineAsync(SelectedDisk, _payloadRoot, progress);
            StatusText = "Boot motoru güncellendi";
            await _logger.InfoAsync($"Boot motoru güncellendi. Fiziksel disk: Disk {result.DiskNumber}");
            _dialogService.ShowInfo(
                "Boot motoru güncellendi",
                $"Yeni BOOTX64.EFI CAYABOOT bölümüne kopyalandı.\n\nFiziksel disk: Disk {result.DiskNumber}\nGüncellenen dosya: {result.BootPayloadPath}");

            await RefreshAsync();
        }
        catch (Exception ex)
        {
            StatusText = "Boot motoru güncelleme hatası";
            await _logger.ErrorAsync("Boot motoru güncellenemedi.", ex);
            _dialogService.ShowError("Boot motoru güncelleme hatası", ex.Message);
        }
        finally
        {
            IsBusy = false;
            RaiseCommandStates();
        }
    }

    private async Task DryRunAsync()
    {
        if (SelectedDisk is null)
        {
            return;
        }

        try
        {
            IsBusy = true;
            RefreshPayloadState();
            StatusText = "Dry run hazırlanıyor";
            var safePlan = await _installService.CreateSafeInstallPlanAsync(SelectedDisk, _payloadRoot);
            var cleanPlan = await _installService.CreateCleanInstallPlanAsync(SelectedDisk, _payloadRoot);

            await _logger.InfoAsync($"Dry run hedefi: {SelectedDisk.DisplayName}");
            await _logger.InfoAsync($"Fiziksel disk: Disk {cleanPlan.DiskNumber}");
            await _logger.InfoAsync($"Boot bölümü geçici harf: {cleanPlan.BootDriveLetter}");
            await _logger.InfoAsync($"Veri bölümü harfi: {cleanPlan.DataDriveLetter}");
            await _logger.InfoAsync($"Veri bölümü dosya sistemi: {cleanPlan.DataFileSystem}");
            await _logger.InfoAsync($"Payload kaynak: {safePlan.BootPayloadSource}");
            await _logger.InfoAsync($"Payload mevcut: {safePlan.BootPayloadExists}");

            StatusText = "Dry run tamamlandı";
            RefreshInstallPrerequisiteText();
            _dialogService.ShowInfo(
                "Dry run tamamlandı",
                $"Diske yazma yapılmadı.\n\nTemiz kurulum hedefi: Disk {cleanPlan.DiskNumber}\nBoot: {cleanPlan.BootDriveLetter} CAYABOOT (sistem)\nVeri: {cleanPlan.DataDriveLetter} CAYAISO ({cleanPlan.DataFileSystem}, boş kullanıcı alanı)");
        }
        catch (Exception ex)
        {
            StatusText = "Dry run hatası";
            await _logger.ErrorAsync("Dry run başarısız oldu.", ex);
            _dialogService.ShowError("Dry run hatası", ex.Message);
        }
        finally
        {
            IsBusy = false;
        }
    }

    private void OpenIsoFolder()
    {
        if (_lastDataRoot is not null && Directory.Exists(_lastDataRoot))
        {
            ShellService.OpenFolder(_lastDataRoot);
            return;
        }

        if (SelectedDisk is null)
        {
            return;
        }

        ShellService.OpenFolder(SelectedDisk.RootPath);
    }

    private async Task SafeEjectAsync()
    {
        var diskNumber = SelectedDisk?.Storage.DiskNumber;
        if (diskNumber is null && _lastDataRoot is not null)
        {
            diskNumber = _safeEjectService.TryGetDiskNumberForRoot(_lastDataRoot);
        }

        if (diskNumber is null)
        {
            _dialogService.ShowError("Güvenli çıkar", "Fiziksel disk numarası bulunamadı. USB'yi yenileyip tekrar seçin.");
            return;
        }

        try
        {
            IsBusy = true;
            StatusText = "USB güvenli çıkarılıyor";
            var result = await _safeEjectService.EjectByDiskNumberAsync(diskNumber.Value);
            StatusText = "USB çıkarılabilir";
            _lastDataRoot = null;

            _dialogService.ShowInfo(
                "USB çıkarılabilir",
                $"mountvol /p tamamlandı.\n\nFiziksel disk: Disk {result.DiskNumber}\nKaldırılan bağlama noktaları: {string.Join(", ", result.RemovedMountPoints)}\n\nUSB'yi fiziksel olarak çıkarabilirsiniz.");

            await RefreshAsync();
        }
        catch (OperationCanceledException)
        {
            StatusText = "Güvenli çıkar iptal edildi";
            await _logger.WarningAsync("Güvenli çıkar yönetici izni iptal edildi.");
        }
        catch (Exception ex)
        {
            StatusText = "Güvenli çıkar hatası";
            await _logger.ErrorAsync("Güvenli çıkar başarısız oldu.", ex);
            _dialogService.ShowError("Güvenli çıkar hatası", ex.Message);
        }
        finally
        {
            IsBusy = false;
            RaiseCommandStates();
        }
    }

    private void OnLogMessageWritten(object? sender, LogMessage message)
    {
        void AddLine()
        {
            LogLines.Add($"[{message.Timestamp:HH:mm:ss}] {FormatSeverity(message.Severity)}: {message.Message}");
            while (LogLines.Count > 300)
            {
                LogLines.RemoveAt(0);
            }
        }

        var dispatcher = Application.Current?.Dispatcher;
        if (dispatcher is null || dispatcher.CheckAccess())
        {
            AddLine();
        }
        else
        {
            dispatcher.Invoke(AddLine);
        }
    }

    private IProgress<InstallProgress> CreateProgress() =>
        new Progress<InstallProgress>(p =>
        {
            ProgressValue = p.Percent;
            StatusText = p.Message;
        });

    private bool ValidatePayloadReady()
    {
        RefreshPayloadState();
        if (PayloadReady)
        {
            return true;
        }

        RefreshInstallPrerequisiteText();
        _dialogService.ShowError(
            "Kurulum hazır değil",
            "BOOTX64.EFI bulunamadı. Projeyi tekrar build edin; boot payload build sırasında otomatik üretilir.");
        return false;
    }

    private bool EnsureAdministratorForCleanInstall()
    {
        if (IsCurrentProcessElevated())
        {
            return true;
        }

        _dialogService.ShowInfo(
            "Yönetici izni gerekli",
            "USB'yi yapılandırmak için yönetici izni gerekiyor. UAC penceresi açılacak; açılan yönetici penceresinde USB'yi tekrar seçip işlemi başlatın.");

        try
        {
            RelaunchAsAdministrator();
            Application.Current.Shutdown();
        }
        catch (Exception ex)
        {
            _ = _logger.ErrorAsync("Yönetici olarak yeniden başlatma başarısız oldu.", ex);
            _dialogService.ShowError("Yönetici izni alınamadı", ex.Message);
        }

        return false;
    }

    private bool EnsureAdministratorForBootEngineUpdate()
    {
        if (IsCurrentProcessElevated())
        {
            return true;
        }

        _dialogService.ShowInfo(
            "Yönetici izni gerekli",
            "Boot motorunu güncellemek için yönetici izni gerekiyor. UAC penceresi açılacak; açılan yönetici penceresinde USB'yi tekrar seçip Boot Motorunu Güncelle düğmesine basın.");

        try
        {
            RelaunchAsAdministrator();
            Application.Current.Shutdown();
        }
        catch (Exception ex)
        {
            _ = _logger.ErrorAsync("Yönetici olarak yeniden başlatma başarısız oldu.", ex);
            _dialogService.ShowError("Yönetici izni alınamadı", ex.Message);
        }

        return false;
    }

    private static bool IsCurrentProcessElevated()
    {
        using var identity = WindowsIdentity.GetCurrent();
        var principal = new WindowsPrincipal(identity);
        return principal.IsInRole(WindowsBuiltInRole.Administrator);
    }

    private static void RelaunchAsAdministrator()
    {
        var processPath = Environment.ProcessPath;
        if (string.IsNullOrWhiteSpace(processPath))
        {
            processPath = Process.GetCurrentProcess().MainModule?.FileName;
        }

        if (string.IsNullOrWhiteSpace(processPath))
        {
            throw new InvalidOperationException("Uygulama yolu bulunamadı.");
        }

        Process.Start(new ProcessStartInfo
        {
            FileName = processPath,
            WorkingDirectory = Environment.CurrentDirectory,
            UseShellExecute = true,
            Verb = "runas"
        });
    }

    private void RaiseCommandStates()
    {
        RefreshCommand.RaiseCanExecuteChanged();
        DryRunCommand.RaiseCanExecuteChanged();
        InstallCommand.RaiseCanExecuteChanged();
        CleanInstallCommand.RaiseCanExecuteChanged();
        UpdateBootEngineCommand.RaiseCanExecuteChanged();
        OpenIsoFolderCommand.RaiseCanExecuteChanged();
        SafeEjectCommand.RaiseCanExecuteChanged();
    }

    private void RefreshPayloadState()
    {
        PayloadReady = File.Exists(Path.Combine(_payloadRoot, "EFI", "BOOT", "BOOTX64.EFI"));
    }

    private void RefreshInstallPrerequisiteText()
    {
        RefreshPayloadState();

        if (SelectedDisk is null)
        {
            InstallPrerequisiteText = PayloadReady
                ? "Hedef USB'yi seçin."
                : "BOOTX64.EFI eksik — projeyi yeniden build edin.";
            return;
        }

        if (!SelectedDisk.Safety.CanInstall)
        {
            InstallPrerequisiteText = SelectedDisk.Safety.DetailText;
            return;
        }

        if (!PayloadReady)
        {
            InstallPrerequisiteText = "BOOTX64.EFI eksik — projeyi yeniden build edin.";
            return;
        }

        InstallPrerequisiteText = SelectedDisk.Storage.DiskNumber is null
            ? "Fiziksel disk numarası doğrulanamadı."
            : $"Hedef: Disk {SelectedDisk.Storage.DiskNumber} · {SelectedDisk.HeaderText}";
    }

    private static string FormatSeverity(LogSeverity severity) =>
        severity switch
        {
            LogSeverity.Info => "Bilgi",
            LogSeverity.Warning => "Uyarı",
            LogSeverity.Error => "Hata",
            _ => severity.ToString()
        };
}
