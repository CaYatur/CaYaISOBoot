using CaYaISOBoot.Core.Interfaces;
using CaYaISOBoot.Core.Models;

namespace CaYaISOBoot.Logging;

public sealed class FileLogger : IAppLogger, IAsyncDisposable
{
    private readonly SemaphoreSlim _gate = new(1, 1);
    private readonly string _logFilePath;

    public FileLogger()
    {
        var logDirectory = Path.Combine(
            Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),
            "CaYaISOBoot",
            "logs");

        Directory.CreateDirectory(logDirectory);
        _logFilePath = Path.Combine(logDirectory, $"installer-{DateTimeOffset.Now:yyyyMMdd}.log");
    }

    public event EventHandler<LogMessage>? MessageWritten;

    public string LogFilePath => _logFilePath;

    public string LogDirectory => Path.GetDirectoryName(_logFilePath)!;

    public Task InfoAsync(string message, CancellationToken cancellationToken = default) =>
        WriteAsync(new LogMessage(DateTimeOffset.Now, LogSeverity.Info, message), cancellationToken);

    public Task WarningAsync(string message, CancellationToken cancellationToken = default) =>
        WriteAsync(new LogMessage(DateTimeOffset.Now, LogSeverity.Warning, message), cancellationToken);

    public Task ErrorAsync(string message, Exception? exception = null, CancellationToken cancellationToken = default) =>
        WriteAsync(new LogMessage(DateTimeOffset.Now, LogSeverity.Error, message, exception), cancellationToken);

    private async Task WriteAsync(LogMessage logMessage, CancellationToken cancellationToken)
    {
        MessageWritten?.Invoke(this, logMessage);

        var line = Format(logMessage);
        await _gate.WaitAsync(cancellationToken).ConfigureAwait(false);
        try
        {
            await File.AppendAllTextAsync(_logFilePath, line + Environment.NewLine, cancellationToken)
                .ConfigureAwait(false);
        }
        finally
        {
            _gate.Release();
        }
    }

    private static string Format(LogMessage message)
    {
        var line = $"{message.Timestamp:O} [{message.Severity}] {message.Message}";
        return message.Exception is null ? line : $"{line} :: {message.Exception}";
    }

    public ValueTask DisposeAsync()
    {
        _gate.Dispose();
        return ValueTask.CompletedTask;
    }
}

