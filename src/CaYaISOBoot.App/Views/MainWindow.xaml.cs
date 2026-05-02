using System.Windows;
using System.IO;
using CaYaISOBoot.App.Services;
using CaYaISOBoot.App.ViewModels;
using CaYaISOBoot.Disk;
using CaYaISOBoot.Logging;

namespace CaYaISOBoot.App.Views;

public partial class MainWindow : Window
{
    private readonly FileLogger _logger;

    public MainWindow()
    {
        InitializeComponent();

        _logger = new FileLogger();
        var safetyService = new DiskSafetyService();
        var scanner = new DiskScanner(safetyService, _logger);
        var installService = new UsbInstallService(_logger);
        var safeEjectService = new SafeEjectService(_logger);
        var dialogService = new WpfDialogService(this);

        DataContext = new MainViewModel(
            scanner,
            installService,
            safeEjectService,
            dialogService,
            _logger,
            ResolvePayloadRoot(),
            _logger.LogDirectory);

        Loaded += OnLoaded;
        Closed += OnClosed;
    }

    private async void OnLoaded(object sender, RoutedEventArgs e)
    {
        if (DataContext is MainViewModel viewModel)
        {
            await viewModel.InitializeAsync();
        }
    }

    private void OnClosed(object? sender, EventArgs e)
    {
        _logger.DisposeAsync().AsTask().GetAwaiter().GetResult();
    }

    private static string ResolvePayloadRoot()
    {
        var candidates = EnumeratePayloadCandidates().Distinct(StringComparer.OrdinalIgnoreCase).ToArray();
        var withBootPayload = candidates.FirstOrDefault(candidate =>
            File.Exists(Path.Combine(candidate, "EFI", "BOOT", "BOOTX64.EFI")));

        if (withBootPayload is not null)
        {
            return withBootPayload;
        }

        return candidates.FirstOrDefault(Directory.Exists) ??
               Path.Combine(AppContext.BaseDirectory, "installer_payload");
    }

    private static IEnumerable<string> EnumeratePayloadCandidates()
    {
        yield return Path.Combine(AppContext.BaseDirectory, "installer_payload");
        yield return Path.Combine(Environment.CurrentDirectory, "installer_payload");

        var directory = new DirectoryInfo(AppContext.BaseDirectory);
        for (var i = 0; i < 8 && directory is not null; i++)
        {
            yield return Path.Combine(directory.FullName, "installer_payload");
            directory = directory.Parent;
        }
    }
}
