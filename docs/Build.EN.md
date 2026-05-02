# Build Instructions

## Windows Installer

Requirements:

- Windows 10/11
- .NET SDK 8 or later
- Windows Desktop runtime with WPF support

Commands:

```powershell
dotnet build CaYaISOBoot.sln -c Release -p:RestoreUseStaticGraphEvaluation=true -m:1
```

Run:

```powershell
dotnet run --project src/CaYaISOBoot.App/CaYaISOBoot.App.csproj
```

The following files must exist for the installer to perform a real installation:

```text
installer_payload/EFI/BOOT/BOOTX64.EFI
installer_payload/CaYaISOBoot/winpe/CaYaWinPeBridge.exe
```

These files are now generated automatically by MSVC when `src/CaYaISOBoot.App`
is built. MSVC x64 tools inside Visual Studio / Build Tools must be installed
for the automatic build to work.

## Configure USB mode

Configure USB mode repartitions the selected physical USB disk:

- 512 MB FAT32 `CAYABOOT`: CaYa boot system and `BOOTX64.EFI`.
- Remaining space `CAYAISO`: empty data partition where no system files are
  written; the user copies ISO/IMG/EFI files here.

This operation erases data. The application:

- Does not pre-select a disk on first launch.
- Verifies the physical disk number.
- Blocks the same physical disk as the Windows system disk.
- Requires a two-step confirmation (informational prompt + final Yes/No).

## UEFI Boot Engine

Requirements:

- EDK2 build environment
- Visual Studio Build Tools or a supported GCC/Clang toolchain
- `edksetup` and `build` commands must be configured

Recommended EDK2 placement:

```text
edk2/
  CaYaISOBootPkg/
    CaYaNativeBootEngine/
      main.c
      ...
      CaYaNativeBootEngine.inf
```

Add as a component in the EDK2 `dsc` file:

```text
[Components]
  CaYaISOBootPkg/CaYaNativeBootEngine/CaYaNativeBootEngine.inf
```

After the build, copy the produced EFI file to:

```text
installer_payload/EFI/BOOT/BOOTX64.EFI
```

## Test checklist

1. `dotnet build` must complete without errors.
2. The application must not list internal disks.
3. When a USB drive is selected, details must be displayed correctly.
4. Safe install must create the folders after two confirmations.
5. Installation must be considered failed if the EFI payload is missing.
6. With the payload present, `/EFI/BOOT/BOOTX64.EFI` must appear on the USB.
7. The menu must open on QEMU/OVMF or a real UEFI machine.
8. Files placed in the CAYAISO root or a user-created `/ISO` folder must be
   listed. `/CaYaISOBoot/ISO` is also scanned for legacy/safe install
   compatibility.
9. Selecting `.efi` must attempt `LoadImage` / `StartImage`.
10. Selecting a Windows `.iso` must create
    `CaYaISOBoot/cache/selected-windows-iso.txt`.
11. The product key screen must not be locked when Windows Setup starts; the
    user must be able to enter their own key or proceed without one if the ISO
    supports it.
12. Extracted `.inf` storage drivers placed under
    `CaYaISOBoot/drivers/winpe/` or `$WinPEDriver$` must be attempted at WinPE
    startup.
