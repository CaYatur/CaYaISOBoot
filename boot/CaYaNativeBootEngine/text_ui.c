#include "text_ui.h"

#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

VOID
CayaClearScreen (
  VOID
  )
{
  gST->ConOut->ClearScreen (gST->ConOut);
}

VOID
CayaPrintHeader (
  VOID
  )
{
  Print (L"CaYaISOBoot\r\n");
  Print (L"Native Multi-ISO USB Boot System\r\n");
  Print (L"CaYa Native Boot Engine\r\n\r\n");
}

VOID
CayaWaitForAnyKey (
  VOID
  )
{
  EFI_INPUT_KEY Key;
  UINTN EventIndex;

  gBS->WaitForEvent (1, &gST->ConIn->WaitForKey, &EventIndex);
  while (gST->ConIn->ReadKeyStroke (gST->ConIn, &Key) == EFI_NOT_READY) {
  }
}

VOID
CayaPrintStatus (
  IN EFI_STATUS Status
  )
{
  if (EFI_ERROR (Status)) {
    Print (L"UEFI status code: %r\r\n", Status);
  } else {
    Print (L"UEFI status code: EFI_SUCCESS\r\n");
  }
}
