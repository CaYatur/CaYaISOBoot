#include "efi_loader.h"
#include "text_ui.h"

#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

EFI_STATUS
CayaStartEfiEntry (
  IN EFI_HANDLE ParentImageHandle,
  IN EFI_HANDLE DeviceHandle,
  IN CAYA_BOOT_ENTRY *Entry
  )
{
  EFI_STATUS Status;
  EFI_DEVICE_PATH_PROTOCOL *DevicePath;
  EFI_HANDLE ChildImage;

  if (ParentImageHandle == NULL || DeviceHandle == NULL || Entry == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DevicePath = FileDevicePath (DeviceHandle, Entry->Path);
  if (DevicePath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  ChildImage = NULL;
  Status = gBS->LoadImage (
                  FALSE,
                  ParentImageHandle,
                  DevicePath,
                  NULL,
                  0,
                  &ChildImage
                  );

  if (EFI_ERROR (Status)) {
    Print (L"EFI image could not be loaded: %s\r\n", Entry->Path);
    CayaPrintStatus (Status);
    FreePool (DevicePath);
    return Status;
  }

  Print (L"Starting EFI image: %s\r\n", Entry->Path);
  Status = gBS->StartImage (ChildImage, NULL, NULL);
  CayaPrintStatus (Status);

  FreePool (DevicePath);
  return Status;
}
