# CaYa Native Boot Engine Build

This MVP is written as an EDK2 UEFI application. EDK2 is used only as the
UEFI application build environment; it is not used as a ready bootloader.

## Files

- `main.c`: UEFI entry point and main event loop.
- `file_scan.c`: Loaded image device lookup and controlled USB directory scan.
- `boot_menu.c`: Simple Text Output menu and keyboard handling.
- `efi_loader.c`: `.efi` chainload via `LoadImage` / `StartImage`.
- `iso_handler.c`: Honest unsupported ISO/IMG handler for MVP.
- `text_ui.c`: Shared console helpers.
- `CaYaNativeBootEngine.inf`: EDK2 module description.

## EDK2 integration

Place this directory under an EDK2 package, for example:

```text
edk2/CaYaISOBootPkg/CaYaNativeBootEngine/
```

Add the INF to your package DSC:

```text
[Components]
  CaYaISOBootPkg/CaYaNativeBootEngine/CaYaNativeBootEngine.inf
```

Build with your configured EDK2 toolchain, for example:

```powershell
build -a X64 -t VS2022 -p CaYaISOBootPkg/CaYaISOBootPkg.dsc -b RELEASE
```

Copy the produced EFI application to:

```text
installer_payload/EFI/BOOT/BOOTX64.EFI
```

## MVP behavior

The boot engine scans:

- `\`
- `\ISO`
- `\CaYaISOBoot\ISO`

It lists `.iso`, `.img` and `.efi` files. `.efi` files are chainloaded.
`.iso` and `.img` files are intentionally not booted in MVP and return an
explicit unsupported message.

