using CaYaISOBoot.Core.Models;
using CaYaISOBoot.Core.Shared;

namespace CaYaISOBoot.ISO;

public sealed class IsoScanner
{
    public IReadOnlyList<BootFileCandidate> ScanUsbRoot(string usbRoot)
    {
        if (string.IsNullOrWhiteSpace(usbRoot))
        {
            throw new ArgumentException("USB root bos olamaz.", nameof(usbRoot));
        }

        var root = Path.GetFullPath(usbRoot);
        var entries = new List<BootFileCandidate>();

        foreach (var relativeDirectory in AppConstants.BootScanDirectories)
        {
            var directory = string.IsNullOrEmpty(relativeDirectory)
                ? root
                : Path.Combine(root, relativeDirectory.Replace('/', Path.DirectorySeparatorChar));

            if (!Directory.Exists(directory))
            {
                continue;
            }

            foreach (var file in Directory.EnumerateFiles(directory))
            {
                var extension = Path.GetExtension(file);
                if (!AppConstants.SupportedBootExtensions.Contains(extension, StringComparer.OrdinalIgnoreCase))
                {
                    continue;
                }

                var info = new FileInfo(file);
                entries.Add(new BootFileCandidate(
                    info.Name,
                    info.FullName,
                    ToBootFileType(extension),
                    info.Length));
            }
        }

        return entries
            .OrderBy(entry => entry.Name, StringComparer.OrdinalIgnoreCase)
            .ToArray();
    }

    private static BootFileType ToBootFileType(string extension) =>
        extension.ToLowerInvariant() switch
        {
            ".iso" => BootFileType.Iso,
            ".img" => BootFileType.RawImage,
            ".efi" => BootFileType.EfiApplication,
            _ => BootFileType.Unknown
        };
}

