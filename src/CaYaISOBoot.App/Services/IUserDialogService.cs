using CaYaISOBoot.Disk;

namespace CaYaISOBoot.App.Services;

public interface IUserDialogService
{
    bool ConfirmSafeInstallIntro(UsbDiskInfo disk);

    bool ConfirmSafeInstallFinal(UsbDiskInfo disk);

    bool ConfirmCleanInstallIntro(UsbDiskInfo disk, CleanInstallPlan plan);

    bool ConfirmCleanInstallFinal(UsbDiskInfo disk, CleanInstallPlan plan);

    bool ConfirmBootEngineUpdate(UsbDiskInfo disk);

    void ShowError(string title, string message);

    void ShowInfo(string title, string message);
}
