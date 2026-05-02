using System.Diagnostics;
using System.IO;

namespace CaYaISOBoot.App.Services;

public static class ShellService
{
    public static void OpenFolder(string path)
    {
        Directory.CreateDirectory(path);
        Process.Start(new ProcessStartInfo
        {
            FileName = path,
            UseShellExecute = true
        });
    }
}
