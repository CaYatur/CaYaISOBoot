using System.Windows;
using System.Windows.Controls;
using CaYaISOBoot.Disk;

namespace CaYaISOBoot.App.Services;

public sealed class WpfDialogService : IUserDialogService
{
    private readonly Window _owner;

    public WpfDialogService(Window owner)
    {
        _owner = owner;
    }

    public bool ConfirmSafeInstallIntro(UsbDiskInfo disk)
    {
        var message = $"""
            CaYaISOBoot bu USB belleğe kurulacak.
            Mevcut dosyalar korunacak.

            Hedef: {disk.DisplayName}
            """;

        return MessageBox.Show(_owner, message, "Güvenli Kurulum", MessageBoxButton.OKCancel, MessageBoxImage.Information) == MessageBoxResult.OK;
    }

    public bool ConfirmSafeInstallFinal(UsbDiskInfo disk)
    {
        var message = $"""
            Kurulumu başlatmadan önce hedefi tekrar kontrol edin.

            Disk: {disk.DisplayName}
            Sürücü: {disk.DriveLetter}
            Kapasite: {disk.CapacityText}
            Dosya sistemi: {disk.FileSystem}
            Mod: Güvenli Kurulum
            Veri silme: Hayır

            OK tuşu kurulumu başlatır.
            """;

        return MessageBox.Show(_owner, message, "Kurulumu Başlat", MessageBoxButton.OKCancel, MessageBoxImage.Question) == MessageBoxResult.OK;
    }

    public bool ConfirmCleanInstallIntro(UsbDiskInfo disk, CleanInstallPlan plan)
    {
        var message = $"""
            Seçilen USB yeniden yapılandırılacak.

            Fiziksel disk: Disk {plan.DiskNumber}
            Mevcut sürücü: {disk.DriveLetter}
            Kapasite: {disk.CapacityText}

            Oluşturulacak yapı:
            - 512 MB FAT32 CAYABOOT bölümü: CaYa boot sistemi
            - Kalan alan {plan.DataFileSystem} CAYAISO bölümü: boş kullanıcı ISO/IMG/EFI alanı

            Bu işlem seçilen USB üzerindeki tüm verileri siler.
            """;

        return MessageBox.Show(_owner, message, "USB'yi Yapılandır", MessageBoxButton.OKCancel, MessageBoxImage.Warning) == MessageBoxResult.OK;
    }

    public bool ConfirmBootEngineUpdate(UsbDiskInfo disk)
    {
        var message = $"""
            CaYa Native Boot Engine güncellenecek.

            Hedef: {disk.DisplayName}
            Fiziksel disk: Disk {disk.Storage.DiskNumber}

            Bu işlem ISO/veri bölümünü formatlamaz.
            Gizli CAYABOOT bölümü geçici olarak bağlanır; BOOTX64.EFI, WinPE bridge ve sistem dosyaları burada güncellenir. CAYAISO veri bölümüne sistem dosyası yazılmaz.
            """;

        return MessageBox.Show(_owner, message, "Boot Motorunu Güncelle", MessageBoxButton.OKCancel, MessageBoxImage.Information) == MessageBoxResult.OK;
    }

    public bool ConfirmCleanInstallFinal(UsbDiskInfo disk, CleanInstallPlan plan)
    {
        var fp = disk.Fingerprint;
        var removableText = (disk.IsRemovable, disk.IsUsbConnected) switch
        {
            (true, true) => "Evet (Removable + USB)",
            (true, false) => "Evet (Removable)",
            (false, true) => "Evet (USB)",
            _ => "HAYIR — DİKKAT"
        };

        var message = $"""
            SON ONAY: Aşağıdaki USB TAMAMEN SİLİNECEK ve yeniden bölümlenecek.

            Disk No        : {plan.DiskNumber}
            Model          : {fp.Vendor ?? "?"} {fp.Product ?? "?"}
            Serial         : {fp.SerialNumber ?? "?"}
            Boyut          : {disk.CapacityText} ({fp.TotalBytes} bayt)
            Bus            : {fp.BusType}
            Removable/USB  : {removableText}
            Sürücü harfi   : {disk.DriveLetter}
            Mevcut FS      : {disk.FileSystem}

            Yapılacak işlem: clean → MBR → 512 MB FAT32 (CAYABOOT, sistem) + kalan {plan.DataFileSystem} (CAYAISO, boş veri alanı)

            Yanlış disk seçtiyseniz şimdi İptal'e basın.
            Devam etmek için Evet'e basın.
            """;

        return MessageBox.Show(
            _owner,
            message,
            "USB Formatlama Son Onayı",
            MessageBoxButton.YesNo,
            MessageBoxImage.Warning,
            MessageBoxResult.No) == MessageBoxResult.Yes;
    }

    public void ShowError(string title, string message) =>
        MessageBox.Show(_owner, message, title, MessageBoxButton.OK, MessageBoxImage.Error);

    public void ShowInfo(string title, string message) =>
        MessageBox.Show(_owner, message, title, MessageBoxButton.OK, MessageBoxImage.Information);
}
