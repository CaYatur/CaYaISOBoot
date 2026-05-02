# CaYaISOBoot Technical Architecture

## A) Realistic technical architecture

CaYaISOBoot consists of two independent but cooperating components:

1. The Windows installer prepares the folder structure and EFI payload on the
   USB drive. The default installation mode does not erase data.
2. The CaYa Native Boot Engine is launched by UEFI firmware as
   `/EFI/BOOT/BOOTX64.EFI`, opens the USB file system, dynamically lists
   bootable files, and handles user selection.

In safe install mode the installer only copies files and creates folders.
Configure USB mode can split the selected USB into two partitions after a
two-step confirmation.

## B) UEFI app approach without a third-party bootloader

UEFI firmware can already run PE/COFF EFI applications from a FAT-based EFI
System Partition. The cleanest approach for the MVP is therefore to write our
own `BOOTX64.EFI`. EDK2 here is not a bootloader but the build and UEFI
header/library infrastructure. Menu, file scanning, EFI chainload, and ISO
handler decisions all stay in CaYa code.

## C) CaYa Native Boot Engine — runtime logic

The boot engine:

1. Finds the device handle it was loaded from via `Loaded Image Protocol`.
2. Opens `Simple File System Protocol` on the same handle.
3. Scans the root directory and target ISO folders.
4. Loads `.iso`, `.img`, `.efi` entries into a `BOOT_ENTRY` list in memory.
5. Draws a Text UI menu and accepts keyboard selection.
6. Attempts `LoadImage` / `StartImage` for `.efi` files.
7. Attempts the ISO9660/El Torito chain for `.iso` files; prepares the WinPE
   bridge flow for Windows ISOs. States the support boundary clearly for `.img`.

On firmware with GOP, the menu header is drawn on a graphics canvas using the
embedded CaYaDev logo. If GOP is not found, the same menu opens text-only via
Simple Text Output.

## D) Ventoy-style dynamic use architecture

The USB is prepared once. In clean install mode the CaYa system files stay in
the CAYABOOT partition; CAYAISO is left as an empty data partition for the
user. The user then copies ISO files to the CAYAISO root or their own `/ISO/`
folder without returning to the Windows application. `/CaYaISOBoot/ISO/` is
also still scanned for safe install and legacy install compatibility.
The boot engine rebuilds the list from the USB file system on every boot.
A pre-made config file is not required. In later releases a prepared config
mode may generate ISO analysis and cache data to improve compatibility.

## E) Windows Installer architecture

Installer layers:

- WPF UI: disk list, detail panel, log panel and commands.
- ViewModel: async commands, selected disk, progress and confirmation flow.
- Disk service: scans USB disks, filters out the system disk and internal disks.
- Install service: creates safe install folders and copies EFI payload.
- Logging service: writes UI and file logs through a single channel.

## F) Safe disk detection system

Detection rules:

- `C:` and the Windows root drive are always blocked.
- Drives that are not ready, whose file system cannot be read, or that are
  system volumes are filtered out.
- Bus type and removable media status are read via Win32
  `DeviceIoControl(IOCTL_STORAGE_QUERY_PROPERTY)`.
- Internal SATA/NVMe/SCSI disks are not listed.
- Large USB-connected disks may be listed but are flagged with an extra warning.
- If the safety check fails, the install command does not run.

## G) Safe installation flow to USB

Safe install:

1. A disk is selected.
2. First confirmation: "CaYaISOBoot will be installed to this USB drive.
   Existing files will be preserved."
3. Second confirmation: disk name, letter, capacity, file system and install mode.
4. Folders are created.
5. `installer_payload/EFI/BOOT/BOOTX64.EFI` is copied to the USB.
6. Installation summary and log are written.

Written verification is only used in format/clean install mode.

## H) USB folder structure

Clean install produces two partitions:

```text
CAYABOOT (FAT32, hidden)
  /EFI/BOOT/BOOTX64.EFI
  /Autounattend.xml
  /CaYaISOBoot/system/
  /CaYaISOBoot/config/
  /CaYaISOBoot/logs/
  /CaYaISOBoot/cache/
  /CaYaISOBoot/drivers/
  /CaYaISOBoot/drivers/winpe/
  /CaYaISOBoot/winpe/CaYaWinPeBridge.exe
  /$WinPEDriver$/

CAYAISO (user-accessible data partition)
  <empty; user copies ISO/IMG/EFI files here or under /ISO>
```

Because safe install is single-partition, the same files are created in the
root of the selected USB.

## I) Dynamic ISO scanning system

The boot engine only scans controlled paths:

- `/`
- `/ISO/`
- `/CaYaISOBoot/ISO/`
