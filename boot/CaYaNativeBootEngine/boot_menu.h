#ifndef CAYA_BOOT_MENU_H
#define CAYA_BOOT_MENU_H

#include <Uefi.h>
#include "file_scan.h"

typedef enum {
  CayaMenuActionBoot = 0,
  CayaMenuActionRefresh,
  CayaMenuActionExit
} CAYA_MENU_ACTION;

typedef struct {
  CAYA_MENU_ACTION Action;
  UINTN SelectedIndex;
} CAYA_MENU_RESULT;

EFI_STATUS
CayaRunBootMenu (
  IN CAYA_BOOT_ENTRY_LIST *Entries,
  OUT CAYA_MENU_RESULT *Result
  );

#endif

