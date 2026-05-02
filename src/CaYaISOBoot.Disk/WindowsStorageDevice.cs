using System.Runtime.InteropServices;
using System.Text;
using Microsoft.Win32.SafeHandles;

namespace CaYaISOBoot.Disk;

internal static class WindowsStorageDevice
{
    private const uint FileShareRead = 0x00000001;
    private const uint FileShareWrite = 0x00000002;
    private const uint OpenExisting = 3;
    private const uint IoctlStorageQueryProperty = 0x002D1400;
    private const uint IoctlVolumeGetVolumeDiskExtents = 0x00560000;

    public static StorageDeviceInfo Query(string rootPath)
    {
        var devicePath = ToVolumeDevicePath(rootPath);

        using var handle = CreateFile(
            devicePath,
            0,
            FileShareRead | FileShareWrite,
            IntPtr.Zero,
            OpenExisting,
            0,
            IntPtr.Zero);

        if (handle.IsInvalid)
        {
            return StorageDeviceInfo.Unknown;
        }

        var query = new byte[12];
        var output = new byte[1024];

        if (!DeviceIoControl(handle, IoctlStorageQueryProperty, query, query.Length, output, output.Length, out var bytesReturned, IntPtr.Zero) ||
            bytesReturned < 36)
        {
            return StorageDeviceInfo.Unknown;
        }

        var removable = output[10] != 0;
        var busTypeValue = BitConverter.ToInt32(output, 28);
        var busType = Enum.IsDefined(typeof(StorageBusType), busTypeValue)
            ? (StorageBusType)busTypeValue
            : StorageBusType.Unknown;

        return new StorageDeviceInfo(
            busType,
            removable,
            QueryDiskNumber(handle),
            ReadAsciiOffset(output, bytesReturned, 12),
            ReadAsciiOffset(output, bytesReturned, 16),
            ReadAsciiOffset(output, bytesReturned, 24));
    }

    private static int? QueryDiskNumber(SafeFileHandle handle)
    {
        var output = new byte[256];
        if (!DeviceIoControl(handle, IoctlVolumeGetVolumeDiskExtents, Array.Empty<byte>(), 0, output, output.Length, out var bytesReturned, IntPtr.Zero) ||
            bytesReturned < 24)
        {
            return null;
        }

        var extentCount = BitConverter.ToInt32(output, 0);
        if (extentCount != 1)
        {
            return null;
        }

        return BitConverter.ToInt32(output, 8);
    }

    private static string ToVolumeDevicePath(string rootPath)
    {
        var trimmed = rootPath.TrimEnd('\\', '/');
        return @"\\.\" + trimmed;
    }

    private static string? ReadAsciiOffset(byte[] buffer, int validLength, int offsetField)
    {
        if (validLength < offsetField + sizeof(int))
        {
            return null;
        }

        var offset = BitConverter.ToInt32(buffer, offsetField);
        if (offset <= 0 || offset >= validLength)
        {
            return null;
        }

        var end = offset;
        while (end < validLength && buffer[end] != 0)
        {
            end++;
        }

        if (end <= offset)
        {
            return null;
        }

        return Encoding.ASCII.GetString(buffer, offset, end - offset).Trim();
    }

    [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    private static extern SafeFileHandle CreateFile(
        string lpFileName,
        uint dwDesiredAccess,
        uint dwShareMode,
        IntPtr lpSecurityAttributes,
        uint dwCreationDisposition,
        uint dwFlagsAndAttributes,
        IntPtr hTemplateFile);

    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern bool DeviceIoControl(
        SafeFileHandle hDevice,
        uint dwIoControlCode,
        byte[] lpInBuffer,
        int nInBufferSize,
        byte[] lpOutBuffer,
        int nOutBufferSize,
        out int lpBytesReturned,
        IntPtr lpOverlapped);
}
