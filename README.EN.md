# CaYaISOBoot

**Native Multi-ISO USB Boot System** — An original multi-ISO USB boot manager running in a UEFI environment.

---

## What is it?

CaYaISOBoot is a two-component system that can directly boot multiple `.iso`,
`.img`, and `.efi` files from a single USB drive under UEFI firmware.

- **Windows side:** A .NET 8 WPF-based installer application prepares the USB
  drive, copies the boot payload, and repartitions the disk when needed.
- **UEFI side:** `CaYaNativeBootEngine`, compiled with EDK2, is launched by
  firmware as `BOOTX64.EFI` at startup, scans the USB file system, and
  presents the user with a boot menu.

No third-party bootloader is used. Both components are entirely original code.

---

## Features

- Dynamically lists `.iso`, `.img`, and `.efi` files on the USB at boot time — no config file needs to be created beforehand.
- Directly chains `.efi` files via UEFI `LoadImage` / `StartImage`.
- For Windows installation ISOs, remounts the ISO read-only via the Windows Virtual Disk API inside WinPE (`CaYaWinPeBridge`).
- Graphical header with embedded CaYaDev logo on devices with UEFI GOP; automatic text-only mode when GOP is absent.
- **Configure USB** mode: splits the USB into `CAYABOOT` (512 MB FAT32, hidden, boot system) and `CAYAISO` (remaining space, user data partition).
- Safe install mode does not touch existing data — only creates the required folders and copies the EFI payload.
- Supports loading Intel RST/VMD, RAID, NVMe, and chipset storage drivers at WinPE startup.
- `Autounattend.xml` flow that does not lock the product key screen.

---

## Project Structure

```
CaYaISOBoot/
├── src/
│   ├── CaYaISOBoot.App/        # .NET 8 WPF installer application
│   ├── CaYaISOBoot.Core/       # Shared models, constants and interfaces
│   ├── CaYaISOBoot.Disk/       # USB disk detection, safety checks and installation
│   ├── CaYaISOBoot.ISO/        # ISO/IMG/EFI scanning and analysis
│   ├── CaYaISOBoot.Logging/    # File and UI logging infrastructure
│   └── CaYaISOBoot.Tests/      # Unit tests
├── boot/
│   └── CaYaNativeBootEngine/   # UEFI application compiled with EDK2 (C)
├── winpe/
│   └── CaYaWinPeBridge/        # ISO mounting and driver loading inside WinPE (C)
├── installer_payload/
│   └── EFI/BOOT/BOOTX64.EFI   # EFI payload copied to the USB
└── docs/                       # Architecture, security, build and compatibility docs
```

---

## Components

### CaYaISOBoot.App
.NET 8 WPF-based installer application built with MVVM architecture.
Includes a disk list, detail panel, progress bar, and log panel.
Provides safe installation through async commands and a two-step confirmation flow.

### CaYaNativeBootEngine
A fully original UEFI application compiled with the EDK2 infrastructure. Loaded
by firmware as `/EFI/BOOT/BOOTX64.EFI`. The boot engine:

1. Locates the device handle it was loaded from via `Loaded Image Protocol`.
2. Scans the USB root directory and `ISO/` folder via `Simple File System Protocol`.
3. Loads `.iso`, `.img`, `.efi` entries into a `BOOT_ENTRY` list in memory.
4. Draws a keyboard-navigable menu for the user.
5. Launches `LoadImage/StartImage` (EFI) or the ISO9660/El Torito chain based on selection.
6. Prepares the WinPE bridge flow when a Windows ISO is selected.

### CaYaWinPeBridge
The bridge component that prevents Windows installation media from getting lost
during the WinPE phase. Reads the selected ISO path from the cache, mounts it
read-only via the Windows Virtual Disk API, and recursively scans `.inf`
packages under `/CaYaISOBoot/drivers/winpe/` and `/$WinPEDriver$/` to attempt
loading storage drivers.

---

## USB Folder Structure

**After clean install (Configure USB):**

```
CAYABOOT (FAT32, 512 MB, hidden)
  /EFI/BOOT/BOOTX64.EFI
  /Autounattend.xml
  /CaYaISOBoot/system/
  /CaYaISOBoot/config/
  /CaYaISOBoot/logs/
  /CaYaISOBoot/cache/
  /CaYaISOBoot/drivers/winpe/

CAYAISO (remaining space, user data partition)
  /                  ← ISO/IMG/EFI files can be copied directly here
  /ISO/              ← or into this subfolder
```

**After safe install:** The same folder structure is created inside the existing single partition.

---

## Installation and Build

### Requirements

- Windows 10/11
- .NET SDK 8 or later
- Windows Desktop Runtime with WPF support
- For the UEFI boot engine: EDK2 build environment and MSVC x64 tools

### Windows Installer

```powershell
# Build
dotnet build CaYaISOBoot.sln -c Release

# Run
dotnet run --project src/CaYaISOBoot.App/CaYaISOBoot.App.csproj
```

The following files must exist for a real installation to be performed:

```
installer_payload/EFI/BOOT/BOOTX64.EFI
installer_payload/CaYaISOBoot/winpe/CaYaWinPeBridge.exe
```

These files are produced automatically by MSVC when `src/CaYaISOBoot.App` is built with MSVC x64 tools installed.

### UEFI Boot Engine (EDK2)

Place it as `CaYaISOBootPkg/CaYaNativeBootEngine/` inside the EDK2 directory
structure, add it as a component in the `dsc` file, and build. Copy the
produced EFI file to `installer_payload/EFI/BOOT/BOOTX64.EFI`.

Detailed instructions: [docs/Build.EN.md](docs/Build.EN.md)

---

## Security Model

- **No** Secure Boot bypass, signature circumvention, or firmware manipulation.
- `C:` drive and the Windows system disk cannot be an installation target.
- Internal SATA/NVMe/SCSI disks are excluded from the list.
- Safe install does **not** erase existing user data.
- Configure USB mode only repartitions the disk after a two-step confirmation.
- If the EFI payload is missing, the installer does **not** report a successful installation.
- The WinPE bridge only uses the ISO path selected by the user; it does not produce fake success.

Details: [docs/Security.EN.md](docs/Security.EN.md)

---

## Supported File Types

| Type | Behavior |
|------|----------|
| `.efi` | Directly launched via UEFI `LoadImage` / `StartImage` |
| `.iso` | ISO9660/El Torito chain; WinPE bridge is activated for Windows ISOs |
| `.img` | Raw image handler — not yet supported in MVP |

---

## Compatibility

- **First target:** UEFI systems with Secure Boot disabled.
- **Storage drivers:** Intel RST/VMD, RAID, NVMe, or chipset storage packages
  can be placed under `CaYaISOBoot/drivers/winpe/` as extracted `.inf/.sys/.cat` files.
  The `$WinPEDriver$` folder is also supported.
- **Windows Setup product key:** Not filled in automatically thanks to
  `ProductKey/WillShowUI=Always`; the user makes their own choice.

Details: [docs/ISOCompatibility.EN.md](docs/ISOCompatibility.EN.md)

---

## Roadmap

| Phase | Scope |
|-------|-------|
| MVP 1 | Windows installer, secure installation, EFI payload, UEFI text menu, WinPE bridge cache flow |
| MVP 2 | ISO9660 reader, Virtual Disk API mount, WinPE driver loading, ISO metadata cache |
| MVP 3 | El Torito parser, Linux kernel/initrd detection, Ubuntu/Debian/Fedora/Kali profile system |
| MVP 4 | Multi-version Windows ISO compatibility validation, BCD/WIM/ESD detection, error reporting |
| MVP 5 | Legacy BIOS architecture (planning phase) |

Details: [docs/Roadmap.EN.md](docs/Roadmap.EN.md)

---

## License

This project is licensed under the [MIT License](LICENSE).
