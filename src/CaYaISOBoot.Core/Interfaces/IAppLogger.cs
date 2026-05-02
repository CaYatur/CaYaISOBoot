using CaYaISOBoot.Core.Models;

namespace CaYaISOBoot.Core.Interfaces;

public interface IAppLogger
{
    event EventHandler<LogMessage>? MessageWritten;

    Task InfoAsync(string message, CancellationToken cancellationToken = default);

    Task WarningAsync(string message, CancellationToken cancellationToken = default);

    Task ErrorAsync(string message, Exception? exception = null, CancellationToken cancellationToken = default);
}

