#ifndef CAYA_EFI_LOADER_H
#define CAYA_EFI_LOADER_H

#include <Uefi.h>
#include "file_scan.h"

EFI_STATUS
CayaStartEfiEntry (
  IN EFI_HANDLE ParentImageHandle,
  IN EFI_HANDLE DeviceHandle,
  IN CAYA_BOOT_ENTRY *Entry
  );

#endif

