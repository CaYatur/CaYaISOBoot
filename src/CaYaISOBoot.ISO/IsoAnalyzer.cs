using CaYaISOBoot.Core.Models;

namespace CaYaISOBoot.ISO;

public sealed class IsoAnalyzer
{
    public IsoAnalysisResult Analyze(BootFileCandidate candidate)
    {
        return candidate.Type switch
        {
            BootFileType.EfiApplication => new IsoAnalysisResult(
                candidate,
                IsoSupportState.DirectEfiBoot,
                "UEFI uygulaması doğrudan LoadImage/StartImage ile başlatılabilir.",
                "EfiFileHandler"),

            BootFileType.Iso => new IsoAnalysisResult(
                candidate,
                IsoSupportState.AnalysisRequired,
                "ISO algilandi. Boot motoru ISO9660/El Torito zincirini ve Windows icin WinPE ISO bridge akisini kullanir.",
                "VentoyStyleIsoHandler"),

            BootFileType.RawImage => new IsoAnalysisResult(
                candidate,
                IsoSupportState.UnsupportedInMvp,
                "IMG algılandı. Raw image boot desteği MVP dışındadır.",
                "RawImageHandler"),

            _ => new IsoAnalysisResult(
                candidate,
                IsoSupportState.Unknown,
                "Dosya türü desteklenmiyor.",
                "UnknownHandler")
        };
    }
}
