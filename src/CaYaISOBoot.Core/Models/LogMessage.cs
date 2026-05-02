namespace CaYaISOBoot.Core.Models;

public enum LogSeverity
{
    Info,
    Warning,
    Error
}

public sealed record LogMessage(
    DateTimeOffset Timestamp,
    LogSeverity Severity,
    string Message,
    Exception? Exception = null);

