#ifndef CAYA_FILE_SCAN_H
#define CAYA_FILE_SCAN_H

#include <Uefi.h>
#include <Protocol/SimpleFileSystem.h>

#define CAYA_MAX_BOOT_ENTRIES 128
#define CAYA_MAX_NAME_CHARS 128
#define CAYA_MAX_PATH_CHARS 260

typedef enum {
  CayaBootFileUnknown = 0,
  CayaBootFileIso,
  CayaBootFileImg,
  CayaBootFileEfi
} CAYA_BOOT_FILE_TYPE;

typedef struct {
  CHAR16 Name[CAYA_MAX_NAME_CHARS];
  CHAR16 Path[CAYA_MAX_PATH_CHARS];
  UINT64 Size;
  CAYA_BOOT_FILE_TYPE Type;
} CAYA_BOOT_ENTRY;

typedef struct {
  CAYA_BOOT_ENTRY Entries[CAYA_MAX_BOOT_ENTRIES];
  UINTN Count;
  BOOLEAN Truncated;
} CAYA_BOOT_ENTRY_LIST;

typedef struct {
  EFI_HANDLE ImageHandle;
  EFI_HANDLE DeviceHandle;
  EFI_FILE_PROTOCOL *Root;
} CAYA_FILE_SCAN_CONTEXT;

VOID
CayaInitializeBootEntryList (
  OUT CAYA_BOOT_ENTRY_LIST *List
  );

EFI_STATUS
CayaInitializeFileScan (
  IN EFI_HANDLE ImageHandle,
  OUT CAYA_FILE_SCAN_CONTEXT *Context
  );

VOID
CayaCloseFileScan (
  IN OUT CAYA_FILE_SCAN_CONTEXT *Context
  );

EFI_STATUS
CayaScanBootFiles (
  IN CAYA_FILE_SCAN_CONTEXT *Context,
  OUT CAYA_BOOT_ENTRY_LIST *List
  );

CONST CHAR16 *
CayaBootFileTypeName (
  IN CAYA_BOOT_FILE_TYPE Type
  );

#endif

