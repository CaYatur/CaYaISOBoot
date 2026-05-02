# ISO Compatibility Notes

## Current status

The boot engine chains ISO files via ISO9660/El Torito. For Windows
installation ISOs, the CaYaWinPeBridge tool is additionally installed; it
mounts the selected ISO using Windows's own Virtual Disk API inside WinPE.
This allows Windows Setup to see the installation media again, similar to
how Ventoy works.

The `Autounattend.xml` written to the USB root when Windows installation starts
is solely to activate CaYaWinPeBridge and the WinPE driver path. The product
key is not filled in automatically; `ProductKey/WillShowUI=Always` is used so
that Windows Setup shows its own key/edition selection screen.

Folder for additional storage drivers:

```text
/CaYaISOBoot/drivers/winpe/
/$WinPEDriver$/
```

Intel RST/VMD, RAID, NVMe, or chipset storage packages can be placed here as
extracted `.inf/.sys/.cat` files. CaYaWinPeBridge recursively scans this folder
and attempts to load `.inf` files at WinPE startup.
`/CaYaISOBoot/drivers/winpe/` is also declared as a Windows Setup DriverPath;
`/$WinPEDriver$/` is Windows Setup's own standard scan path.

## File types

- `.efi`: Direct UEFI image launch is attempted.
- `.iso`: Passed to the ISO handler; WinPE bridge is activated for Windows ISOs.
- `.img`: Passed to the raw image handler; not supported in MVP.

## Future compatibility goals

- Generate kernel/initrd and boot parameters for Linux live ISOs.
- Add an alternative mount path similar to ImDisk for older Windows 7/WinPE
  versions in the Windows ISO bridge.
- Present error conditions to the user in a clear and technically accurate way.
