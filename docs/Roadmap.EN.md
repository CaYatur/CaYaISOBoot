# Roadmap

## MVP 1

- Secure installation with the Windows installer.
- Protection for C:, the system disk, and internal disks.
- USB folder structure.
- EFI payload copy.
- UEFI text menu.
- Dynamic `.iso`, `.img`, `.efi` scanning.
- `.efi` chainload.
- WinPE bridge cache flow for Windows ISO selection.

## MVP 2

- ISO9660 reader.
- Detection of `/EFI/BOOT/BOOTX64.EFI` inside an ISO.
- Read-only mounting of a Windows ISO inside WinPE via Virtual Disk API.
- Loading WinPE storage drivers from `.inf` packages.
- Autounattend flow that does not lock the product key screen.
- ISO metadata cache.
- ISO analysis tool on the installer side.

## MVP 3

- El Torito boot catalog parser.
- Linux kernel/initrd detection.
- Profile system for Ubuntu, Debian, Fedora, Kali.
- Boot parameter generation.

## MVP 4

- Validate Windows ISO compatibility with more versions.
- Surface `bootmgfw.efi`, BCD, `install.wim` / `install.esd` detections in the UI.
- Merge Windows PE/installer error reports with application logs.
- Large file and multi-partition strategy.

## MVP 5

- Legacy BIOS architecture design.
- 16-bit stage1, stage2 loader, disk reading and text menu.
- FAT/exFAT/NTFS reader plans.
- This phase is not implemented within the current MVP scope.
