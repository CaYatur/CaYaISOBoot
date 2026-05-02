#ifndef CAYA_TEXT_UI_H
#define CAYA_TEXT_UI_H

#include <Uefi.h>

VOID
CayaClearScreen (
  VOID
  );

VOID
CayaPrintHeader (
  VOID
  );

VOID
CayaWaitForAnyKey (
  VOID
  );

VOID
CayaPrintStatus (
  IN EFI_STATUS Status
  );

#endif

