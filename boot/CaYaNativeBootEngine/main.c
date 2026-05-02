#include <Uefi.h>

#include <Library/UefiLib.h>

#include "boot_menu.h"
#include "efi_loader.h"
#include "file_scan.h"
#include "iso_handler.h"
#include "text_ui.h"

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS Status;
  CAYA_FILE_SCAN_CONTEXT ScanContext;

  (VOID)SystemTable;

  CayaClearScreen ();
  CayaPrintHeader ();
  Print (L"Opening USB file systems...\r\n");

  Status = CayaInitializeFileScan (ImageHandle, &ScanContext);
  if (EFI_ERROR (Status)) {
    Print (L"Boot USB filesystem could not be opened.\r\n");
    CayaPrintStatus (Status);
    Print (L"\r\nPress any key to continue.\r\n");
    CayaWaitForAnyKey ();
    return Status;
  }

  for (;;) {
    CAYA_BOOT_ENTRY_LIST Entries;
    CAYA_MENU_RESULT MenuResult;

    Status = CayaScanBootFiles (&ScanContext, &Entries);
    if (EFI_ERROR (Status)) {
      CayaClearScreen ();
      CayaPrintHeader ();
      Print (L"An error occurred while scanning files.\r\n");
      CayaPrintStatus (Status);
      Print (L"\r\nPress any key to continue.\r\n");
      CayaWaitForAnyKey ();
    }

    Status = CayaRunBootMenu (&Entries, &MenuResult);
    if (EFI_ERROR (Status)) {
      break;
    }

    if (MenuResult.Action == CayaMenuActionExit) {
      break;
    }

    if (MenuResult.Action == CayaMenuActionRefresh) {
      continue;
    }

    if (MenuResult.SelectedIndex >= Entries.Count) {
      continue;
    }

    CayaClearScreen ();
    CayaPrintHeader ();

    if (Entries.Entries[MenuResult.SelectedIndex].Type == CayaBootFileEfi) {
      Status = CayaStartEfiEntry (
                 ImageHandle,
                 ScanContext.DeviceHandle,
                 &Entries.Entries[MenuResult.SelectedIndex]
                 );

      Print (L"\r\nEFI chainload attempt returned. Press any key to continue.\r\n");
      CayaWaitForAnyKey ();
    } else {
      (VOID)CayaHandleIsoOrImage (ImageHandle, &ScanContext, &Entries.Entries[MenuResult.SelectedIndex]);
    }
  }

  CayaCloseFileScan (&ScanContext);
  return EFI_SUCCESS;
}
