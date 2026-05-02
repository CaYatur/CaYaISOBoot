namespace CaYaISOBoot.Logging;

public sealed record LogFileInfo(string Path, long SizeBytes, DateTimeOffset LastWriteTime);

