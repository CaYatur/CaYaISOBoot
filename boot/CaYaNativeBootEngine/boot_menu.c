#include "boot_menu.h"
#include "text_ui.h"

#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

STATIC
VOID
CayaPrintEntryStatus (
  IN CAYA_BOOT_ENTRY *Entry
  )
{
  switch (Entry->Type) {
    case CayaBootFileEfi:
      Print (L"State: Direct EFI boot");
      break;
    case CayaBootFileIso:
      Print (L"State: UEFI ISO boot (El Torito / EFI path)");
      break;
    case CayaBootFileImg:
      Print (L"State: Raw image handler not yet implemented");
      break;
    default:
      Print (L"State: Unknown");
      break;
  }
}

STATIC
VOID
CayaDrawMenu (
  IN CAYA_BOOT_ENTRY_LIST *Entries,
  IN UINTN SelectedIndex
  )
{
  UINTN Index;

  CayaClearScreen ();
  CayaPrintHeader ();
  Print (L"Boot files found:\r\n\r\n");

  if (Entries->Count == 0) {
    Print (L"No .iso, .img or .efi files were found on this USB.\r\n\r\n");
  }

  for (Index = 0; Index < Entries->Count; Index++) {
    CAYA_BOOT_ENTRY *Entry;

    Entry = &Entries->Entries[Index];
    Print (Index == SelectedIndex ? L"> " : L"  ");
    Print (L"[%u] %s\r\n", (UINT32)(Index + 1), Entry->Name);
    Print (L"    Type: %s\r\n", CayaBootFileTypeName (Entry->Type));
    Print (L"    Path: %s\r\n", Entry->Path);
    Print (L"    ");
    CayaPrintEntryStatus (Entry);
    Print (L"\r\n\r\n");
  }

  if (Entries->Truncated) {
    Print (L"Note: the list reached the maximum entry limit and was truncated.\r\n\r\n");
  }

  Print (L"Keys: Up/Down select, Enter boot, R rescan, I info, Esc exit\r\n");
}

STATIC
VOID
CayaShowEntryInfo (
  IN CAYA_BOOT_ENTRY *Entry
  )
{
  CayaClearScreen ();
  CayaPrintHeader ();
  Print (L"File information\r\n\r\n");
  Print (L"Name: %s\r\n", Entry->Name);
  Print (L"Path: %s\r\n", Entry->Path);
  Print (L"Type: %s\r\n", CayaBootFileTypeName (Entry->Type));
  Print (L"Size: %Lu bytes\r\n", Entry->Size);
  Print (L"\r\n");
  CayaPrintEntryStatus (Entry);
  Print (L"\r\n\r\nPress any key to continue.\r\n");
  CayaWaitForAnyKey ();
}

EFI_STATUS
CayaRunBootMenu (
  IN CAYA_BOOT_ENTRY_LIST *Entries,
  OUT CAYA_MENU_RESULT *Result
  )
{
  EFI_INPUT_KEY Key;
  UINTN SelectedIndex;

  if (Entries == NULL || Result == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  SelectedIndex = 0;
  Result->Action = CayaMenuActionExit;
  Result->SelectedIndex = 0;

  for (;;) {
    UINTN EventIndex;

    if (Entries->Count > 0 && SelectedIndex >= Entries->Count) {
      SelectedIndex = Entries->Count - 1;
    }

    CayaDrawMenu (Entries, SelectedIndex);

    gBS->WaitForEvent (1, &gST->ConIn->WaitForKey, &EventIndex);
    if (gST->ConIn->ReadKeyStroke (gST->ConIn, &Key) == EFI_NOT_READY) {
      continue;
    }

    if (Key.ScanCode == SCAN_ESC) {
      Result->Action = CayaMenuActionExit;
      return EFI_SUCCESS;
    }

    if (Key.ScanCode == SCAN_UP && Entries->Count > 0) {
      SelectedIndex = (SelectedIndex == 0) ? Entries->Count - 1 : SelectedIndex - 1;
      continue;
    }

    if (Key.ScanCode == SCAN_DOWN && Entries->Count > 0) {
      SelectedIndex = (SelectedIndex + 1) % Entries->Count;
      continue;
    }

    if (Key.UnicodeChar == L'r' || Key.UnicodeChar == L'R') {
      Result->Action = CayaMenuActionRefresh;
      return EFI_SUCCESS;
    }

    if ((Key.UnicodeChar == L'i' || Key.UnicodeChar == L'I') && Entries->Count > 0) {
      CayaShowEntryInfo (&Entries->Entries[SelectedIndex]);
      continue;
    }

    if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN && Entries->Count > 0) {
      Result->Action = CayaMenuActionBoot;
      Result->SelectedIndex = SelectedIndex;
      return EFI_SUCCESS;
    }
  }
}
