using CaYaISOBoot.Core.Models;

namespace CaYaISOBoot.ISO;

public sealed record BootFileCandidate(
    string Name,
    string FullPath,
    BootFileType Type,
    long SizeBytes);

public enum IsoSupportState
{
    DirectEfiBoot,
    AnalysisRequired,
    UnsupportedInMvp,
    Unknown
}

public sealed record IsoAnalysisResult(
    BootFileCandidate Candidate,
    IsoSupportState State,
    string Status,
    string RequiredHandler);

