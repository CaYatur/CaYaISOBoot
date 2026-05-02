#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <virtdisk.h>
#include <stdio.h>
#include <stdarg.h>
#include <wchar.h>

#define CAYA_MARKER_PATH L"X:\\CaYaISOBoot.bridge.done"
#define CAYA_LOG_PATH    L"X:\\CaYaISOBoot.bridge.log"

static const GUID CayaVirtualStorageVendorMicrosoft =
    { 0xec984aec, 0xa0f9, 0x47e9, { 0x90, 0x1f, 0x71, 0x41, 0x5a, 0x66, 0x34, 0x5b } };

static void LogLine(const wchar_t *Format, ...)
{
    FILE *File;
    va_list Args;

    if (_wfopen_s(&File, CAYA_LOG_PATH, L"a, ccs=UTF-8") != 0 || File == NULL) {
        return;
    }

    fwprintf(File, L"[CaYaWinPeBridge] ");
    va_start(Args, Format);
    vfwprintf(File, Format, Args);
    va_end(Args);
    fwprintf(File, L"\n");
    fclose(File);
}

static int FileExists(const wchar_t *Path)
{
    DWORD Attributes = GetFileAttributesW(Path);
    return Attributes != INVALID_FILE_ATTRIBUTES && (Attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

static int DirectoryExists(const wchar_t *Path)
{
    DWORD Attributes = GetFileAttributesW(Path);
    return Attributes != INVALID_FILE_ATTRIBUTES && (Attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

static DWORD RunProcessWait(const wchar_t *CommandLineText)
{
    wchar_t CommandLine[2048];
    STARTUPINFOW StartupInfo;
    PROCESS_INFORMATION ProcessInfo;
    DWORD WaitResult;
    DWORD ExitCode = 1;

    wcsncpy_s(CommandLine, ARRAYSIZE(CommandLine), CommandLineText, _TRUNCATE);

    ZeroMemory(&StartupInfo, sizeof(StartupInfo));
    StartupInfo.cb = sizeof(StartupInfo);
    StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
    StartupInfo.wShowWindow = SW_HIDE;
    ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));

    if (!CreateProcessW(NULL, CommandLine, NULL, NULL, FALSE, CREATE_NO_WINDOW,
                        NULL, NULL, &StartupInfo, &ProcessInfo)) {
        ExitCode = GetLastError();
        LogLine(L"CreateProcess failed for [%s], error=%lu", CommandLineText, ExitCode);
        return ExitCode;
    }

    WaitResult = WaitForSingleObject(ProcessInfo.hProcess, 120000);
    if (WaitResult == WAIT_TIMEOUT) {
        TerminateProcess(ProcessInfo.hProcess, 1460);
        ExitCode = 1460;
        LogLine(L"Process timed out: %s", CommandLineText);
    } else if (!GetExitCodeProcess(ProcessInfo.hProcess, &ExitCode)) {
        ExitCode = GetLastError();
        LogLine(L"GetExitCodeProcess failed for [%s], error=%lu", CommandLineText, ExitCode);
    }

    CloseHandle(ProcessInfo.hThread);
    CloseHandle(ProcessInfo.hProcess);
    return ExitCode;
}

static int IsInfFileName(const wchar_t *FileName)
{
    const wchar_t *Dot = wcsrchr(FileName, L'.');
    return Dot != NULL && _wcsicmp(Dot, L".inf") == 0;
}

static int LoadDriverInf(const wchar_t *InfPath)
{
    wchar_t CommandLine[2048];
    DWORD ExitCode;

    _snwprintf_s(CommandLine, ARRAYSIZE(CommandLine), _TRUNCATE,
                 L"drvload.exe \"%s\"", InfPath);
    ExitCode = RunProcessWait(CommandLine);
    if (ExitCode == ERROR_SUCCESS || ExitCode == ERROR_SUCCESS_REBOOT_REQUIRED) {
        LogLine(L"Loaded WinPE driver with drvload: %s", InfPath);
        return 1;
    }

    LogLine(L"drvload failed for %s, exit=%lu; trying pnputil", InfPath, ExitCode);
    _snwprintf_s(CommandLine, ARRAYSIZE(CommandLine), _TRUNCATE,
                 L"pnputil.exe /add-driver \"%s\" /install", InfPath);
    ExitCode = RunProcessWait(CommandLine);
    if (ExitCode == ERROR_SUCCESS || ExitCode == ERROR_SUCCESS_REBOOT_REQUIRED) {
        LogLine(L"Loaded WinPE driver with pnputil: %s", InfPath);
        return 1;
    }

    LogLine(L"Driver load failed for %s, exit=%lu", InfPath, ExitCode);
    return 0;
}

static void LoadDriverDirectoryRecursive(const wchar_t *Directory, int *LoadedCount)
{
    wchar_t Pattern[MAX_PATH];
    wchar_t Candidate[MAX_PATH];
    WIN32_FIND_DATAW FindData;
    HANDLE Find;

    _snwprintf_s(Pattern, ARRAYSIZE(Pattern), _TRUNCATE, L"%s\\*", Directory);
    Find = FindFirstFileW(Pattern, &FindData);
    if (Find == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        if (wcscmp(FindData.cFileName, L".") == 0 || wcscmp(FindData.cFileName, L"..") == 0) {
            continue;
        }

        _snwprintf_s(Candidate, ARRAYSIZE(Candidate), _TRUNCATE,
                     L"%s\\%s", Directory, FindData.cFileName);

        if ((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
            LoadDriverDirectoryRecursive(Candidate, LoadedCount);
            continue;
        }

        if (IsInfFileName(FindData.cFileName) && LoadDriverInf(Candidate)) {
            (*LoadedCount)++;
        }
    } while (FindNextFileW(Find, &FindData));

    FindClose(Find);
}

static void LoadWinPeDrivers(void)
{
    DWORD Drives = GetLogicalDrives();
    wchar_t Letter;
    wchar_t Directory[MAX_PATH];
    int DriverRoots = 0;
    int Loaded = 0;

    for (Letter = L'C'; Letter <= L'Z'; Letter++) {
        if ((Drives & (1u << (Letter - L'A'))) == 0) {
            continue;
        }

        _snwprintf_s(Directory, ARRAYSIZE(Directory), _TRUNCATE,
                     L"%c:\\CaYaISOBoot\\drivers\\winpe", Letter);
        if (DirectoryExists(Directory)) {
            DriverRoots++;
            LogLine(L"Scanning WinPE driver directory: %s", Directory);
            LoadDriverDirectoryRecursive(Directory, &Loaded);
        }

        _snwprintf_s(Directory, ARRAYSIZE(Directory), _TRUNCATE,
                     L"%c:\\$WinPEDriver$", Letter);
        if (!DirectoryExists(Directory)) {
            continue;
        }

        DriverRoots++;
        LogLine(L"Scanning standard Windows Setup driver directory: %s", Directory);
        LoadDriverDirectoryRecursive(Directory, &Loaded);
    }

    if (DriverRoots == 0) {
        LogLine(L"No WinPE driver directory found.");
    } else {
        LogLine(L"WinPE driver scan completed. roots=%d loaded=%d", DriverRoots, Loaded);
    }
}

static void TouchMarker(void)
{
    HANDLE File = CreateFileW(CAYA_MARKER_PATH, GENERIC_WRITE, FILE_SHARE_READ, NULL,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (File != INVALID_HANDLE_VALUE) {
        const char Text[] = "CaYaISOBoot WinPE bridge started\r\n";
        DWORD Written = 0;
        WriteFile(File, Text, (DWORD)(sizeof(Text) - 1), &Written, NULL);
        CloseHandle(File);
    }
}

static void BuildDriveRoot(wchar_t Letter, wchar_t *Root, DWORD RootChars)
{
    if (RootChars >= 4) {
        Root[0] = Letter;
        Root[1] = L':';
        Root[2] = L'\\';
        Root[3] = 0;
    }
}

static void JoinDrivePath(wchar_t Letter, const wchar_t *RelativeOrAbsolute, wchar_t *Out, DWORD OutChars)
{
    const wchar_t *Path = RelativeOrAbsolute;

    if (OutChars == 0) {
        return;
    }

    if (Path[0] == L'\\' || Path[0] == L'/') {
        Path++;
    }

    _snwprintf_s(Out, OutChars, _TRUNCATE, L"%c:\\%s", Letter, Path);
}

static void TrimLine(wchar_t *Text)
{
    size_t Length = wcslen(Text);
    while (Length > 0 &&
           (Text[Length - 1] == L'\r' || Text[Length - 1] == L'\n' ||
            Text[Length - 1] == L' ' || Text[Length - 1] == L'\t')) {
        Text[Length - 1] = 0;
        Length--;
    }
}

static int ReadSelectedHintPath(wchar_t *SelectedPath, DWORD SelectedPathChars)
{
    DWORD Drives = GetLogicalDrives();
    wchar_t HintPath[MAX_PATH];
    wchar_t Line[512];
    wchar_t Letter;
    FILE *File;

    for (Letter = L'C'; Letter <= L'Z'; Letter++) {
        if ((Drives & (1u << (Letter - L'A'))) == 0) {
            continue;
        }

        _snwprintf_s(HintPath, ARRAYSIZE(HintPath), _TRUNCATE,
                     L"%c:\\CaYaISOBoot\\cache\\selected-windows-iso.txt", Letter);
        if (!FileExists(HintPath)) {
            continue;
        }

        if (_wfopen_s(&File, HintPath, L"r, ccs=UTF-8") != 0 || File == NULL) {
            continue;
        }

        while (fgetws(Line, ARRAYSIZE(Line), File) != NULL) {
            TrimLine(Line);
            if (_wcsnicmp(Line, L"path=", 5) == 0 && Line[5] != 0) {
                wcsncpy_s(SelectedPath, SelectedPathChars, Line + 5, _TRUNCATE);
                fclose(File);
                LogLine(L"Selected hint found at %s -> %s", HintPath, SelectedPath);
                return 1;
            }
        }

        fclose(File);
    }

    return 0;
}

static int ResolveSelectedHint(wchar_t *IsoPath, DWORD IsoPathChars)
{
    wchar_t Hint[512];
    DWORD Drives;
    wchar_t Letter;
    wchar_t Candidate[MAX_PATH];

    if (!ReadSelectedHintPath(Hint, ARRAYSIZE(Hint))) {
        return 0;
    }

    if (wcslen(Hint) > 2 && Hint[1] == L':') {
        if (FileExists(Hint)) {
            wcsncpy_s(IsoPath, IsoPathChars, Hint, _TRUNCATE);
            return 1;
        }
        return 0;
    }

    Drives = GetLogicalDrives();
    for (Letter = L'C'; Letter <= L'Z'; Letter++) {
        if ((Drives & (1u << (Letter - L'A'))) == 0) {
            continue;
        }

        JoinDrivePath(Letter, Hint, Candidate, ARRAYSIZE(Candidate));
        if (FileExists(Candidate)) {
            wcsncpy_s(IsoPath, IsoPathChars, Candidate, _TRUNCATE);
            LogLine(L"Resolved selected ISO: %s", IsoPath);
            return 1;
        }
    }

    LogLine(L"Selected ISO hint could not be resolved: %s", Hint);
    return 0;
}

static DWORD AttachIso(const wchar_t *IsoPath, DWORD AttachFlags, HANDLE *OutHandle)
{
    VIRTUAL_STORAGE_TYPE StorageType;
    OPEN_VIRTUAL_DISK_PARAMETERS OpenParameters;
    ATTACH_VIRTUAL_DISK_PARAMETERS AttachParameters;
    HANDLE Handle = INVALID_HANDLE_VALUE;
    DWORD Error;

    ZeroMemory(&StorageType, sizeof(StorageType));
    StorageType.DeviceId = VIRTUAL_STORAGE_TYPE_DEVICE_ISO;
    StorageType.VendorId = CayaVirtualStorageVendorMicrosoft;

    ZeroMemory(&OpenParameters, sizeof(OpenParameters));
    OpenParameters.Version = OPEN_VIRTUAL_DISK_VERSION_1;

    Error = OpenVirtualDisk(&StorageType,
                            IsoPath,
                            VIRTUAL_DISK_ACCESS_READ | VIRTUAL_DISK_ACCESS_ATTACH_RO,
                            OPEN_VIRTUAL_DISK_FLAG_NONE,
                            &OpenParameters,
                            &Handle);
    if (Error != ERROR_SUCCESS) {
        LogLine(L"OpenVirtualDisk failed for %s, error=%lu", IsoPath, Error);
        return Error;
    }

    ZeroMemory(&AttachParameters, sizeof(AttachParameters));
    AttachParameters.Version = ATTACH_VIRTUAL_DISK_VERSION_1;

    Error = AttachVirtualDisk(Handle,
                              NULL,
                              (ATTACH_VIRTUAL_DISK_FLAG)AttachFlags,
                              0,
                              &AttachParameters,
                              NULL);
    if (Error != ERROR_SUCCESS) {
        LogLine(L"AttachVirtualDisk failed for %s, flags=0x%lx, error=%lu", IsoPath, AttachFlags, Error);
        CloseHandle(Handle);
        return Error;
    }

    *OutHandle = Handle;
    LogLine(L"AttachVirtualDisk succeeded for %s, flags=0x%lx", IsoPath, AttachFlags);
    return ERROR_SUCCESS;
}

static int FindMountedWindowsMedia(wchar_t *MediaRoot, DWORD MediaRootChars)
{
    DWORD Drives = GetLogicalDrives();
    wchar_t Letter;
    wchar_t Root[4];
    wchar_t SetupPath[MAX_PATH];
    wchar_t WimPath[MAX_PATH];
    wchar_t EsdPath[MAX_PATH];
    wchar_t SwmPath[MAX_PATH];

    for (Letter = L'C'; Letter <= L'Z'; Letter++) {
        if ((Drives & (1u << (Letter - L'A'))) == 0) {
            continue;
        }

        BuildDriveRoot(Letter, Root, ARRAYSIZE(Root));
        if (!DirectoryExists(Root)) {
            continue;
        }

        _snwprintf_s(SetupPath, ARRAYSIZE(SetupPath), _TRUNCATE, L"%c:\\setup.exe", Letter);
        _snwprintf_s(WimPath, ARRAYSIZE(WimPath), _TRUNCATE, L"%c:\\sources\\install.wim", Letter);
        _snwprintf_s(EsdPath, ARRAYSIZE(EsdPath), _TRUNCATE, L"%c:\\sources\\install.esd", Letter);
        _snwprintf_s(SwmPath, ARRAYSIZE(SwmPath), _TRUNCATE, L"%c:\\sources\\install.swm", Letter);

        if (FileExists(SetupPath) &&
            (FileExists(WimPath) || FileExists(EsdPath) || FileExists(SwmPath))) {
            wcsncpy_s(MediaRoot, MediaRootChars, Root, _TRUNCATE);
            return 1;
        }
    }

    return 0;
}

static int WaitForMountedWindowsMedia(wchar_t *MediaRoot, DWORD MediaRootChars)
{
    int Attempt;
    for (Attempt = 0; Attempt < 40; Attempt++) {
        if (FindMountedWindowsMedia(MediaRoot, MediaRootChars)) {
            LogLine(L"Windows media visible at %s", MediaRoot);
            return 1;
        }
        Sleep(500);
    }

    LogLine(L"Mounted Windows media was not found.");
    return 0;
}

static int StartHolderProcess(const wchar_t *IsoPath)
{
    wchar_t Self[MAX_PATH];
    wchar_t CommandLine[1024];
    STARTUPINFOW StartupInfo;
    PROCESS_INFORMATION ProcessInfo;

    if (GetModuleFileNameW(NULL, Self, ARRAYSIZE(Self)) == 0) {
        return 0;
    }

    _snwprintf_s(CommandLine, ARRAYSIZE(CommandLine), _TRUNCATE,
                 L"\"%s\" --hold \"%s\"", Self, IsoPath);

    ZeroMemory(&StartupInfo, sizeof(StartupInfo));
    StartupInfo.cb = sizeof(StartupInfo);
    StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
    StartupInfo.wShowWindow = SW_HIDE;
    ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));

    if (!CreateProcessW(NULL, CommandLine, NULL, NULL, FALSE, CREATE_NO_WINDOW,
                        NULL, NULL, &StartupInfo, &ProcessInfo)) {
        LogLine(L"Failed to start holder process, error=%lu", GetLastError());
        return 0;
    }

    CloseHandle(ProcessInfo.hThread);
    CloseHandle(ProcessInfo.hProcess);
    LogLine(L"Holder process started.");
    return 1;
}

static int AttachSelectedIso(const wchar_t *IsoPath)
{
    HANDLE Handle = INVALID_HANDLE_VALUE;
    DWORD Flags = ATTACH_VIRTUAL_DISK_FLAG_READ_ONLY | ATTACH_VIRTUAL_DISK_FLAG_PERMANENT_LIFETIME;
    DWORD Error;
    wchar_t MediaRoot[4];

    Error = AttachIso(IsoPath, Flags, &Handle);
    if (Error == ERROR_SUCCESS) {
        CloseHandle(Handle);
        return WaitForMountedWindowsMedia(MediaRoot, ARRAYSIZE(MediaRoot));
    }

    LogLine(L"Permanent ISO attach failed, falling back to holder process.");
    if (!StartHolderProcess(IsoPath)) {
        return 0;
    }

    return WaitForMountedWindowsMedia(MediaRoot, ARRAYSIZE(MediaRoot));
}

static int TryCandidateIso(const wchar_t *IsoPath)
{
    LogLine(L"Trying ISO candidate: %s", IsoPath);
    return AttachSelectedIso(IsoPath);
}

static int ScanDirectoryForIso(const wchar_t *Directory)
{
    wchar_t Pattern[MAX_PATH];
    WIN32_FIND_DATAW FindData;
    HANDLE Find;
    wchar_t Candidate[MAX_PATH];

    _snwprintf_s(Pattern, ARRAYSIZE(Pattern), _TRUNCATE, L"%s\\*.iso", Directory);
    Find = FindFirstFileW(Pattern, &FindData);
    if (Find == INVALID_HANDLE_VALUE) {
        return 0;
    }

    do {
        if ((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
            continue;
        }

        _snwprintf_s(Candidate, ARRAYSIZE(Candidate), _TRUNCATE,
                     L"%s\\%s", Directory, FindData.cFileName);
        if (TryCandidateIso(Candidate)) {
            FindClose(Find);
            return 1;
        }
    } while (FindNextFileW(Find, &FindData));

    FindClose(Find);
    return 0;
}

static int FallbackScanForWindowsIso(void)
{
    DWORD Drives = GetLogicalDrives();
    wchar_t Letter;
    wchar_t Directory[MAX_PATH];

    for (Letter = L'C'; Letter <= L'Z'; Letter++) {
        if ((Drives & (1u << (Letter - L'A'))) == 0) {
            continue;
        }

        _snwprintf_s(Directory, ARRAYSIZE(Directory), _TRUNCATE, L"%c:\\ISO", Letter);
        if (DirectoryExists(Directory) && ScanDirectoryForIso(Directory)) {
            return 1;
        }

        _snwprintf_s(Directory, ARRAYSIZE(Directory), _TRUNCATE, L"%c:\\CaYaISOBoot\\ISO", Letter);
        if (DirectoryExists(Directory) && ScanDirectoryForIso(Directory)) {
            return 1;
        }

        _snwprintf_s(Directory, ARRAYSIZE(Directory), _TRUNCATE, L"%c:\\", Letter);
        if (DirectoryExists(Directory) && ScanDirectoryForIso(Directory)) {
            return 1;
        }
    }

    return 0;
}

static int HolderMain(const wchar_t *IsoPath)
{
    HANDLE Handle = INVALID_HANDLE_VALUE;
    DWORD Error;

    Error = AttachIso(IsoPath, ATTACH_VIRTUAL_DISK_FLAG_READ_ONLY, &Handle);
    if (Error != ERROR_SUCCESS) {
        return 1;
    }

    LogLine(L"Holder process keeping ISO attached.");
    Sleep(6u * 60u * 60u * 1000u);
    CloseHandle(Handle);
    return 0;
}

int wmain(int argc, wchar_t **argv)
{
    wchar_t IsoPath[MAX_PATH];

    if (argc >= 3 && wcscmp(argv[1], L"--hold") == 0) {
        return HolderMain(argv[2]);
    }

    if (FileExists(CAYA_MARKER_PATH)) {
        LogLine(L"Bridge already ran; exiting.");
        return 0;
    }

    LogLine(L"Bridge started.");
    LoadWinPeDrivers();

    ZeroMemory(IsoPath, sizeof(IsoPath));
    if (ResolveSelectedHint(IsoPath, ARRAYSIZE(IsoPath))) {
        if (AttachSelectedIso(IsoPath)) {
            TouchMarker();
            LogLine(L"Selected ISO attached.");
            return 0;
        }
    }

    LogLine(L"Selected hint unavailable or failed; scanning for Windows ISO.");
    if (FallbackScanForWindowsIso()) {
        TouchMarker();
        LogLine(L"Fallback ISO attached.");
        return 0;
    }

    LogLine(L"No Windows ISO could be attached.");
    return 1;
}
