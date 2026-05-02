#include "file_scan.h"

#include <Guid/FileInfo.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/LoadedImage.h>

STATIC CONST CHAR16 *mScanDirectories[] = {
  L"\\",
  L"\\ISO",
  L"\\CaYaISOBoot\\ISO"
};

STATIC
CAYA_BOOT_FILE_TYPE
CayaDetectFileType (
  IN CONST CHAR16 *FileName
  )
{
  CONST CHAR16 *Ext;
  UINTN Index;

  Ext = NULL;
  for (Index = 0; FileName[Index] != L'\0'; Index++) {
    if (FileName[Index] == L'.') {
      Ext = &FileName[Index];
    }
  }

  if (Ext == NULL) {
    return CayaBootFileUnknown;
  }

  if (StriCmp (Ext, L".iso") == 0) {
    return CayaBootFileIso;
  }

  if (StriCmp (Ext, L".img") == 0) {
    return CayaBootFileImg;
  }

  if (StriCmp (Ext, L".efi") == 0) {
    return CayaBootFileEfi;
  }

  return CayaBootFileUnknown;
}

STATIC
EFI_STATUS
CayaBuildEntryPath (
  IN CONST CHAR16 *Directory,
  IN CONST CHAR16 *FileName,
  OUT CHAR16 *Path,
  IN UINTN PathChars
  )
{
  EFI_STATUS Status;

  if (StrCmp (Directory, L"\\") == 0) {
    Status = StrCpyS (Path, PathChars, L"\\");
  } else {
    Status = StrCpyS (Path, PathChars, Directory);
    if (!EFI_ERROR (Status)) {
      Status = StrCatS (Path, PathChars, L"\\");
    }
  }

  if (EFI_ERROR (Status)) {
    return Status;
  }

  return StrCatS (Path, PathChars, FileName);
}

STATIC
EFI_STATUS
CayaAddBootEntry (
  IN OUT CAYA_BOOT_ENTRY_LIST *List,
  IN CONST CHAR16 *Directory,
  IN EFI_FILE_INFO *FileInfo,
  IN CAYA_BOOT_FILE_TYPE Type
  )
{
  EFI_STATUS Status;
  CAYA_BOOT_ENTRY *Entry;

  if (List->Count >= CAYA_MAX_BOOT_ENTRIES) {
    List->Truncated = TRUE;
    return EFI_BUFFER_TOO_SMALL;
  }

  Entry = &List->Entries[List->Count];
  ZeroMem (Entry, sizeof (*Entry));

  Status = StrCpyS (Entry->Name, CAYA_MAX_NAME_CHARS, FileInfo->FileName);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = CayaBuildEntryPath (Directory, FileInfo->FileName, Entry->Path, CAYA_MAX_PATH_CHARS);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Entry->Size = FileInfo->FileSize;
  Entry->Type = Type;
  List->Count++;
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
CayaOpenScanDirectory (
  IN EFI_FILE_PROTOCOL *Root,
  IN CONST CHAR16 *Path,
  OUT EFI_FILE_PROTOCOL **Directory
  )
{
  if (StrCmp (Path, L"\\") == 0) {
    *Directory = Root;
    return EFI_SUCCESS;
  }

  return Root->Open (Root, Directory, (CHAR16 *)Path, EFI_FILE_MODE_READ, 0);
}

STATIC
EFI_STATUS
CayaScanDirectory (
  IN EFI_FILE_PROTOCOL *Root,
  IN CONST CHAR16 *DirectoryPath,
  IN OUT CAYA_BOOT_ENTRY_LIST *List
  )
{
  EFI_STATUS Status;
  EFI_FILE_PROTOCOL *Directory;
  EFI_FILE_INFO *Info;
  UINTN BufferSize;

  Status = CayaOpenScanDirectory (Root, DirectoryPath, &Directory);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  BufferSize = SIZE_OF_EFI_FILE_INFO + (CAYA_MAX_NAME_CHARS * sizeof (CHAR16));
  Info = AllocateZeroPool (BufferSize);
  if (Info == NULL) {
    if (Directory != Root) {
      Directory->Close (Directory);
    }
    return EFI_OUT_OF_RESOURCES;
  }

  for (;;) {
    UINTN ReadSize;
    CAYA_BOOT_FILE_TYPE Type;

    ZeroMem (Info, BufferSize);
    ReadSize = BufferSize;
    Status = Directory->Read (Directory, &ReadSize, Info);
    if (EFI_ERROR (Status) || ReadSize == 0) {
      break;
    }

    if ((Info->Attribute & EFI_FILE_DIRECTORY) != 0) {
      continue;
    }

    Type = CayaDetectFileType (Info->FileName);
    if (Type == CayaBootFileUnknown) {
      continue;
    }

    Status = CayaAddBootEntry (List, DirectoryPath, Info, Type);
    if (Status == EFI_BUFFER_TOO_SMALL) {
      Status = EFI_SUCCESS;
      break;
    }

    if (EFI_ERROR (Status)) {
      break;
    }
  }

  FreePool (Info);
  if (Directory != Root) {
    Directory->Close (Directory);
  }

  return EFI_ERROR (Status) ? Status : EFI_SUCCESS;
}

VOID
CayaInitializeBootEntryList (
  OUT CAYA_BOOT_ENTRY_LIST *List
  )
{
  ZeroMem (List, sizeof (*List));
}

EFI_STATUS
CayaInitializeFileScan (
  IN EFI_HANDLE ImageHandle,
  OUT CAYA_FILE_SCAN_CONTEXT *Context
  )
{
  EFI_STATUS Status;
  EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystem;

  ZeroMem (Context, sizeof (*Context));
  Context->ImageHandle = ImageHandle;

  Status = gBS->HandleProtocol (
                  ImageHandle,
                  &gEfiLoadedImageProtocolGuid,
                  (VOID **)&LoadedImage
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Context->DeviceHandle = LoadedImage->DeviceHandle;

  Status = gBS->HandleProtocol (
                  Context->DeviceHandle,
                  &gEfiSimpleFileSystemProtocolGuid,
                  (VOID **)&SimpleFileSystem
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return SimpleFileSystem->OpenVolume (SimpleFileSystem, &Context->Root);
}

VOID
CayaCloseFileScan (
  IN OUT CAYA_FILE_SCAN_CONTEXT *Context
  )
{
  if (Context->Root != NULL) {
    Context->Root->Close (Context->Root);
    Context->Root = NULL;
  }
}

EFI_STATUS
CayaScanBootFiles (
  IN CAYA_FILE_SCAN_CONTEXT *Context,
  OUT CAYA_BOOT_ENTRY_LIST *List
  )
{
  EFI_STATUS LastError;
  UINTN Index;

  CayaInitializeBootEntryList (List);
  if (Context == NULL || Context->Root == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  LastError = EFI_SUCCESS;
  for (Index = 0; Index < ARRAY_SIZE (mScanDirectories); Index++) {
    EFI_STATUS Status;

    Status = CayaScanDirectory (Context->Root, mScanDirectories[Index], List);
    if (EFI_ERROR (Status) && Status != EFI_NOT_FOUND) {
      LastError = Status;
    }
  }

  return LastError;
}

CONST CHAR16 *
CayaBootFileTypeName (
  IN CAYA_BOOT_FILE_TYPE Type
  )
{
  switch (Type) {
    case CayaBootFileIso:
      return L"ISO";
    case CayaBootFileImg:
      return L"IMG";
    case CayaBootFileEfi:
      return L"EFI";
    default:
      return L"UNKNOWN";
  }
}

