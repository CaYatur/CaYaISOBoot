namespace CaYaISOBoot.Core.Shared;

public static class ByteFormatter
{
    private static readonly string[] Units = ["B", "KB", "MB", "GB", "TB"];

    public static string Format(ulong bytes)
    {
        var value = (double)bytes;
        var unit = 0;

        while (value >= 1024 && unit < Units.Length - 1)
        {
            value /= 1024;
            unit++;
        }

        return $"{value:0.##} {Units[unit]}";
    }
}

