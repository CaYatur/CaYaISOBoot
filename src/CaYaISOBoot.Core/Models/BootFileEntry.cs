namespace CaYaISOBoot.Core.Models;

public sealed record BootFileEntry(
    string Name,
    string FullPath,
    BootFileType Type,
    long SizeBytes)
{
    public string Status => Type switch
    {
        BootFileType.EfiApplication => "Doğrudan başlatılabilir",
        BootFileType.Iso => "ISO Handler gerekli",
        BootFileType.RawImage => "Raw image handler gerekli",
        _ => "Bilinmeyen dosya türü"
    };
}
