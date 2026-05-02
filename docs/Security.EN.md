# Security Model

## Core rules

- No Secure Boot bypass, signature circumvention, or firmware manipulation.
- Safe install does not erase user data; Configure USB mode repartitions the
  selected USB only after a two-step confirmation.
- The system disk, `C:` drive, and the Windows root drive cannot be a
  installation target.
- Internal disks are excluded from the disk list entirely.
- If the EFI payload is missing, the installer does not report a successful
  installation.

## Disk protection

The installer performs the following checks for every candidate disk:

- Is the volume ready?
- Is the drive letter the system root drive?
- Is the bus type USB, or is the media removable?
- Can the file system be read?
- Is the capacity unexpectedly large?

If any blocking condition is found, the install command does not run.

## Secure Boot

The first release targets UEFI systems with Secure Boot disabled. For Secure
Boot support, EFI binary signing, key distribution, and the correct trust
model must be documented in a future release.

## ISO boot security

For Windows ISOs, the WinPE bridge only writes the ISO path the user selected
from the menu to the cache, then mounts the same ISO read-only inside Windows
PE. The bridge attempts driver loading only from extracted `.inf` packages on
the USB; it does not bypass signatures or Secure Boot. It does not produce
a fake success for unsupported ISO paths.
