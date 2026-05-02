/**
  CaYaISOBoot – ISO Handler
  Implements ISO 9660 + El Torito parsing and UEFI chainload from ISO images.

  Boot strategy (in order of preference):
    1. El Torito boot catalog – locate a UEFI (platform 0xEF) boot image,
       load it into memory and execute via LoadImage/StartImage.
    2. If no El Torito UEFI entry exists, walk the ISO 9660 directory tree
       looking for /EFI/BOOT/BOOTX64.EFI, extract it into an AllocatedPool
       buffer and load it with LoadImage from RAM.
**/

#include "iso_handler.h"
#include "text_ui.h"

#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/BlockIo.h>

// -----------------------------------------------------------------------
// Internal helpers – raw sector I/O via EFI_BLOCK_IO_PROTOCOL
// -----------------------------------------------------------------------

STATIC
EFI_STATUS
CayaOpenBlockIo (
  IN  EFI_HANDLE           DeviceHandle,
  OUT EFI_BLOCK_IO_PROTOCOL **BlockIo
  )
{
  return gBS->HandleProtocol (
                DeviceHandle,
                &gEfiBlockIoProtocolGuid,
                (VOID **)BlockIo
                );
}

/**
  Read one or more consecutive 2048-byte ISO sectors from the block device.

  @param BlockIo   Block I/O protocol instance.
  @param Lba       Logical block address of the first sector to read.
  @param Count     Number of sectors to read.
  @param Buffer    Caller-allocated buffer; must be at least Count * 2048 bytes.
**/
STATIC
EFI_STATUS
CayaReadIsoSectors (
  IN  EFI_BLOCK_IO_PROTOCOL *BlockIo,
  IN  UINT32                Lba,
  IN  UINTN                 Count,
  OUT VOID                  *Buffer
  )
{
  UINT32 MediaBlockSize;
  UINT64 ByteOffset;
  UINTN  ByteCount;

  MediaBlockSize = BlockIo->Media->BlockSize;
  ByteOffset     = (UINT64)Lba * ISO9660_SECTOR_SIZE;
  ByteCount      = Count * ISO9660_SECTOR_SIZE;

  if (MediaBlockSize == ISO9660_SECTOR_SIZE) {
    // Direct read – media block size matches ISO sector size
    return BlockIo->ReadBlocks (
                      BlockIo,
                      BlockIo->Media->MediaId,
                      (EFI_LBA)Lba,
                      ByteCount,
                      Buffer
                      );
  }

  // Media block size differs (e.g. 512-byte sectors on a USB mass storage
  // device that wraps a raw ISO).  Convert byte offset / length to media LBAs.
  {
    EFI_LBA  StartLba;
    EFI_LBA  EndLba;
    UINTN    ReadBytes;
    UINT8    *AlignedBuf;
    EFI_STATUS Status;

    StartLba  = ByteOffset / MediaBlockSize;
    EndLba    = (ByteOffset + ByteCount - 1) / MediaBlockSize;
    ReadBytes = (UINTN)((EndLba - StartLba + 1) * MediaBlockSize);

    AlignedBuf = AllocatePool (ReadBytes);
    if (AlignedBuf == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    Status = BlockIo->ReadBlocks (
                        BlockIo,
                        BlockIo->Media->MediaId,
                        StartLba,
                        ReadBytes,
                        AlignedBuf
                        );
    if (!EFI_ERROR (Status)) {
      CopyMem (
        Buffer,
        AlignedBuf + (UINTN)(ByteOffset - StartLba * MediaBlockSize),
        ByteCount
        );
    }

    FreePool (AlignedBuf);
    return Status;
  }
}

// -----------------------------------------------------------------------
// El Torito – find the UEFI boot image LBA and size
// -----------------------------------------------------------------------

/**
  Parse the El Torito boot catalog and return the LBA + sector count of the
  first bootable UEFI (platform 0xEF) image entry.  Falls back to x86 "no
  emulation" if no UEFI entry is present.

  @param BlockIo          Block I/O for the ISO device.
  @param CatalogLba       LBA of the boot catalog (from Boot Record VD).
  @param OutImageLba      Receives the LBA of the boot image.
  @param OutSectorCount   Receives the number of 512-byte virtual sectors to load.
                          0 means "load until EOF / use a safe default".
  @param OutPlatformId    Receives the platform ID of the chosen entry.

  @retval EFI_SUCCESS     A bootable entry was found.
  @retval EFI_NOT_FOUND   No bootable entry found.
**/
STATIC
EFI_STATUS
CayaParseElToritoCatalog (
  IN  EFI_BLOCK_IO_PROTOCOL *BlockIo,
  IN  UINT32                CatalogLba,
  OUT UINT32                *OutImageLba,
  OUT UINT16                *OutSectorCount,
  OUT UINT8                 *OutPlatformId
  )
{
  EFI_STATUS Status;
  UINT8      *Catalog;
  UINTN      Offset;
  UINT8      PlatformId;

  // The catalog is typically a single 2048-byte sector, but can span more.
  // Reading two sectors is sufficient for any realistic catalog.
  Catalog = AllocatePool (ISO9660_SECTOR_SIZE * 2);
  if (Catalog == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = CayaReadIsoSectors (BlockIo, CatalogLba, 2, Catalog);
  if (EFI_ERROR (Status)) {
    FreePool (Catalog);
    return Status;
  }

  // --- Offset 0: Validation Entry ---
  {
    ELTORITO_VALIDATION_ENTRY *Val = (ELTORITO_VALIDATION_ENTRY *)Catalog;
    if (Val->HeaderId != 0x01 || Val->Key55 != 0x55 || Val->KeyAA != 0xAA) {
      FreePool (Catalog);
      return EFI_NOT_FOUND;
    }
    PlatformId = Val->PlatformId; // platform of the initial/default entry
  }

  // --- Offset 32: Initial/Default Entry ---
  {
    ELTORITO_DEFAULT_ENTRY *Def = (ELTORITO_DEFAULT_ENTRY *)(Catalog + 32);
    if (Def->Bootable == ELTORITO_BOOTABLE && PlatformId == ELTORITO_PLATFORM_UEFI) {
      *OutImageLba    = Def->LoadRba;
      *OutSectorCount = Def->SectorCount;
      *OutPlatformId  = PlatformId;
      FreePool (Catalog);
      return EFI_SUCCESS;
    }
  }

  // --- Walk section headers / section entries looking for a UEFI entry ---
  Offset = 64; // first section header starts at byte 64
  while (Offset + 32 <= ISO9660_SECTOR_SIZE * 2) {
    ELTORITO_SECTION_HEADER *Hdr = (ELTORITO_SECTION_HEADER *)(Catalog + Offset);
    UINT8   HdrIndicator = Hdr->HeaderIndicator;
    UINT8   SectionPlatform;
    UINT16  NumEntries;
    UINTN   EntryIndex;

    if (HdrIndicator != 0x90 && HdrIndicator != 0x91) {
      break; // not a valid section header
    }

    SectionPlatform = Hdr->PlatformId;
    NumEntries      = Hdr->NumEntries;
    Offset         += 32; // skip header to first entry

    for (EntryIndex = 0; EntryIndex < NumEntries; EntryIndex++) {
      if (Offset + 32 > ISO9660_SECTOR_SIZE * 2) {
        break;
      }

      ELTORITO_SECTION_ENTRY *Entry = (ELTORITO_SECTION_ENTRY *)(Catalog + Offset);

      if (Entry->Bootable == ELTORITO_BOOTABLE &&
          SectionPlatform == ELTORITO_PLATFORM_UEFI) {
        *OutImageLba    = Entry->LoadRba;
        *OutSectorCount = Entry->SectorCount;
        *OutPlatformId  = SectionPlatform;
        FreePool (Catalog);
        return EFI_SUCCESS;
      }

      Offset += 32;
    }

    if (HdrIndicator == 0x91) {
      break; // last section
    }
  }

  // No UEFI entry – try the default x86 no-emulation entry as a last resort
  {
    ELTORITO_DEFAULT_ENTRY *Def = (ELTORITO_DEFAULT_ENTRY *)(Catalog + 32);
    if (Def->Bootable == ELTORITO_BOOTABLE && Def->MediaType == 0) {
      *OutImageLba    = Def->LoadRba;
      *OutSectorCount = Def->SectorCount;
      *OutPlatformId  = PlatformId;
      FreePool (Catalog);
      return EFI_SUCCESS;
    }
  }

  FreePool (Catalog);
  return EFI_NOT_FOUND;
}

// -----------------------------------------------------------------------
// ISO 9660 directory traversal – locate /EFI/BOOT/BOOTX64.EFI
// -----------------------------------------------------------------------

STATIC
BOOLEAN
CayaIso9660NamesEqual (
  IN CONST UINT8  *IsoName,   // upper-case, may have ";1" suffix
  IN UINTN        IsoNameLen,
  IN CONST CHAR8  *AsciiTarget
  )
{
  UINTN TargetLen = AsciiStrLen (AsciiTarget);
  UINTN CompareLen = IsoNameLen;

  // Strip ";1" version suffix if present
  if (CompareLen >= 2 &&
      IsoName[CompareLen - 2] == ';' &&
      IsoName[CompareLen - 1] == '1') {
    CompareLen -= 2;
  }

  if (CompareLen != TargetLen) {
    return FALSE;
  }

  UINTN i;
  for (i = 0; i < TargetLen; i++) {
    CHAR8 a = (CHAR8)IsoName[i];
    CHAR8 b = AsciiTarget[i];
    // Case-insensitive compare
    if (a >= 'a' && a <= 'z') a -= 32;
    if (b >= 'a' && b <= 'z') b -= 32;
    if (a != b) return FALSE;
  }
  return TRUE;
}

/**
  Look up a single path component (file or directory name) inside an ISO 9660
  directory whose extent starts at @DirectoryLba and is @DirectoryLength bytes.

  @param BlockIo         Block I/O for the ISO.
  @param DirectoryLba    LBA of the directory extent.
  @param DirectoryLength Size in bytes of the directory extent.
  @param Name            ASCII name to look for (case-insensitive, without ";1").
  @param IsDir           TRUE if we expect a directory entry, FALSE for a file.
  @param OutLba          Receives the LBA of the found entry's data extent.
  @param OutLength       Receives the data length in bytes.
**/
STATIC
EFI_STATUS
CayaIso9660FindEntry (
  IN  EFI_BLOCK_IO_PROTOCOL *BlockIo,
  IN  UINT32                DirectoryLba,
  IN  UINT32                DirectoryLength,
  IN  CONST CHAR8           *Name,
  IN  BOOLEAN               IsDir,
  OUT UINT32                *OutLba,
  OUT UINT32                *OutLength
  )
{
  EFI_STATUS Status;
  UINT32     SectorCount;
  UINT8      *DirData;
  UINT32     Offset;

  SectorCount = (DirectoryLength + ISO9660_SECTOR_SIZE - 1) / ISO9660_SECTOR_SIZE;
  DirData     = AllocatePool ((UINTN)SectorCount * ISO9660_SECTOR_SIZE);
  if (DirData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = CayaReadIsoSectors (BlockIo, DirectoryLba, SectorCount, DirData);
  if (EFI_ERROR (Status)) {
    FreePool (DirData);
    return Status;
  }

  Status = EFI_NOT_FOUND;
  Offset = 0;

  while (Offset < DirectoryLength) {
    ISO9660_DIR_RECORD *Rec = (ISO9660_DIR_RECORD *)(DirData + Offset);

    if (Rec->Length == 0) {
      // Padding – skip to next sector boundary
      UINT32 NextSector = ((Offset / ISO9660_SECTOR_SIZE) + 1) * ISO9660_SECTOR_SIZE;
      if (NextSector >= DirectoryLength) break;
      Offset = NextSector;
      continue;
    }

    // Skip "." and ".." entries (identifier length 1, identifier 0x00 or 0x01)
    if (Rec->FileIdentifierLength == 1) {
      UINT8 Id = *(DirData + Offset + sizeof(ISO9660_DIR_RECORD));
      if (Id == 0x00 || Id == 0x01) {
        Offset += Rec->Length;
        continue;
      }
    }

    BOOLEAN RecIsDir = (Rec->FileFlags & 0x02) != 0;

    if (RecIsDir == IsDir) {
      UINT8 *IdPtr = DirData + Offset + sizeof(ISO9660_DIR_RECORD);
      if (CayaIso9660NamesEqual (IdPtr, Rec->FileIdentifierLength, Name)) {
        *OutLba    = Rec->ExtentLbaLe;
        *OutLength = Rec->DataLengthLe;
        Status = EFI_SUCCESS;
        break;
      }
    }

    Offset += Rec->Length;
  }

  FreePool (DirData);
  return Status;
}

/**
  Walk the ISO 9660 directory tree to find /EFI/BOOT/BOOTX64.EFI.
  Returns its data LBA and size in bytes.
**/
STATIC
EFI_STATUS
CayaFindUefiEfiInIso (
  IN  EFI_BLOCK_IO_PROTOCOL *BlockIo,
  IN  ISO9660_PVD           *Pvd,
  OUT UINT32                *OutLba,
  OUT UINT32                *OutLength
  )
{
  EFI_STATUS Status;
  ISO9660_DIR_RECORD *RootRec;
  UINT32 RootLba, RootLen;
  UINT32 EfiLba, EfiLen;
  UINT32 BootLba, BootLen;

  RootRec = (ISO9660_DIR_RECORD *)Pvd->RootDirEntry;
  RootLba = RootRec->ExtentLbaLe;
  RootLen = RootRec->DataLengthLe;

  // /EFI
  Status = CayaIso9660FindEntry (BlockIo, RootLba, RootLen, "EFI", TRUE, &EfiLba, &EfiLen);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  // /EFI/BOOT
  Status = CayaIso9660FindEntry (BlockIo, EfiLba, EfiLen, "BOOT", TRUE, &BootLba, &BootLen);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  // /EFI/BOOT/BOOTX64.EFI
  Status = CayaIso9660FindEntry (BlockIo, BootLba, BootLen, "BOOTX64.EFI", FALSE, OutLba, OutLength);
  return Status;
}

// -----------------------------------------------------------------------
// Load an EFI image from a raw byte buffer in pool memory
// -----------------------------------------------------------------------

STATIC
EFI_STATUS
CayaLoadAndStartImageFromBuffer (
  IN EFI_HANDLE ParentImageHandle,
  IN VOID       *ImageData,
  IN UINTN      ImageSize
  )
{
  EFI_STATUS                         Status;
  EFI_HANDLE                         ChildImage;
  EFI_DEVICE_PATH_PROTOCOL           *DpNode;

  // Build a minimal MemoryMapped device path so LoadImage can accept a buffer
  DpNode = (EFI_DEVICE_PATH_PROTOCOL *)AllocatePool (
             sizeof (MEMMAP_DEVICE_PATH) + sizeof (EFI_DEVICE_PATH_PROTOCOL)
             );
  if (DpNode == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  {
    MEMMAP_DEVICE_PATH *Mm = (MEMMAP_DEVICE_PATH *)DpNode;
    Mm->Header.Type    = HARDWARE_DEVICE_PATH;
    Mm->Header.SubType = HW_MEMMAP_DP;
    Mm->Header.Length[0] = sizeof (MEMMAP_DEVICE_PATH) & 0xFF;
    Mm->Header.Length[1] = (sizeof (MEMMAP_DEVICE_PATH) >> 8) & 0xFF;
    Mm->MemoryType       = EfiLoaderData;
    Mm->StartingAddress  = (EFI_PHYSICAL_ADDRESS)(UINTN)ImageData;
    Mm->EndingAddress    = Mm->StartingAddress + ImageSize - 1;
  }

  {
    EFI_DEVICE_PATH_PROTOCOL *End =
      (EFI_DEVICE_PATH_PROTOCOL *)((UINT8 *)DpNode + sizeof (MEMMAP_DEVICE_PATH));
    End->Type    = END_DEVICE_PATH_TYPE;
    End->SubType = END_ENTIRE_DEVICE_PATH_SUBTYPE;
    End->Length[0] = sizeof (EFI_DEVICE_PATH_PROTOCOL) & 0xFF;
    End->Length[1] = 0;
  }

  ChildImage = NULL;
  Status = gBS->LoadImage (
                  FALSE,
                  ParentImageHandle,
                  DpNode,
                  ImageData,
                  ImageSize,
                  &ChildImage
                  );
  FreePool (DpNode);

  if (EFI_ERROR (Status)) {
    Print (L"LoadImage failed: ");
    CayaPrintStatus (Status);
    return Status;
  }

  Print (L"Starting EFI image from ISO...\r\n");
  Status = gBS->StartImage (ChildImage, NULL, NULL);
  return Status;
}

// -----------------------------------------------------------------------
// Main ISO boot routine
// -----------------------------------------------------------------------

EFI_STATUS
CayaHandleIsoOrImage (
  IN EFI_HANDLE             ParentImageHandle,
  IN CAYA_FILE_SCAN_CONTEXT *ScanContext,
  IN CAYA_BOOT_ENTRY        *Entry
  )
{
  EFI_STATUS             Status;
  EFI_BLOCK_IO_PROTOCOL  *BlockIo;
  UINT8                  *SectorBuf;
  ISO9660_PVD            *Pvd;
  UINT32                 CatalogLba;
  UINT32                 ImageLba;
  UINT16                 ImageSectorCount;
  UINT8                  PlatformId;
  UINT32                 ImageLength;
  VOID                   *ImageData;

  if (Entry == NULL || ScanContext == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  CayaClearScreen ();
  CayaPrintHeader ();
  Print (L"Selected file : %s\r\n", Entry->Name);
  Print (L"Path          : %s\r\n", Entry->Path);
  Print (L"Type          : %s\r\n\r\n", CayaBootFileTypeName (Entry->Type));

  if (Entry->Type == CayaBootFileImg) {
    Print (L"Raw IMG boot is not yet implemented.\r\n");
    Print (L"\r\nPress any key to return.\r\n");
    CayaWaitForAnyKey ();
    return EFI_UNSUPPORTED;
  }

  if (Entry->Type != CayaBootFileIso) {
    Print (L"This handler only supports ISO and IMG files.\r\n");
    Print (L"\r\nPress any key to return.\r\n");
    CayaWaitForAnyKey ();
    return EFI_INVALID_PARAMETER;
  }

  // ----- Step 1: Open Block I/O for the device -----
  Print (L"Opening block device...\r\n");
  Status = CayaOpenBlockIo (ScanContext->DeviceHandle, &BlockIo);
  if (EFI_ERROR (Status)) {
    Print (L"Block I/O not available: ");
    CayaPrintStatus (Status);
    Print (L"\r\nPress any key to return.\r\n");
    CayaWaitForAnyKey ();
    return Status;
  }

  SectorBuf = AllocatePool (ISO9660_SECTOR_SIZE * 2);
  if (SectorBuf == NULL) {
    Print (L"Out of memory.\r\n");
    CayaWaitForAnyKey ();
    return EFI_OUT_OF_RESOURCES;
  }

  // ----- Step 2: Read and verify the Primary Volume Descriptor (sector 16) -----
  Print (L"Reading ISO 9660 Primary Volume Descriptor...\r\n");
  Status = CayaReadIsoSectors (BlockIo, ISO9660_SYSTEM_AREA_LBA, 1, SectorBuf);
  if (EFI_ERROR (Status)) {
    Print (L"Could not read PVD: ");
    CayaPrintStatus (Status);
    FreePool (SectorBuf);
    CayaWaitForAnyKey ();
    return Status;
  }

  Pvd = (ISO9660_PVD *)SectorBuf;
  if (Pvd->TypeCode != 1 ||
      Pvd->Identifier[0] != 'C' ||
      Pvd->Identifier[1] != 'D' ||
      Pvd->Identifier[2] != '0' ||
      Pvd->Identifier[3] != '0' ||
      Pvd->Identifier[4] != '1') {
    Print (L"Invalid ISO 9660 signature. File may be corrupt.\r\n");
    FreePool (SectorBuf);
    CayaWaitForAnyKey ();
    return EFI_VOLUME_CORRUPTED;
  }

  Print (L"ISO 9660 PVD verified.\r\n");

  // Keep a copy of the PVD so we can use it for directory traversal later
  ISO9660_PVD PvdCopy;
  CopyMem (&PvdCopy, Pvd, sizeof (ISO9660_PVD));

  // ----- Step 3: Find El Torito Boot Record VD (sector 17+) -----
  CatalogLba = 0;
  {
    UINT32 VdLba;
    for (VdLba = ISO9660_SYSTEM_AREA_LBA + 1; VdLba < ISO9660_SYSTEM_AREA_LBA + 32; VdLba++) {
      ISO9660_VD_HEADER *Hdr;

      Status = CayaReadIsoSectors (BlockIo, VdLba, 1, SectorBuf);
      if (EFI_ERROR (Status)) {
        break;
      }

      Hdr = (ISO9660_VD_HEADER *)SectorBuf;

      if (Hdr->TypeCode == 255) {
        break; // Volume Descriptor Set Terminator
      }

      if (Hdr->TypeCode == 0) {
        // Boot Record VD
        ISO9660_BOOT_RECORD_VD *Brvd = (ISO9660_BOOT_RECORD_VD *)SectorBuf;
        // Verify El Torito signature
        if (AsciiStrnCmp ((CHAR8 *)Brvd->BootSystemIdentifier,
                           "EL TORITO SPECIFICATION", 23) == 0) {
          CatalogLba = Brvd->BootCatalogLba;
          Print (L"El Torito boot catalog at LBA %u.\r\n", CatalogLba);
          break;
        }
      }
    }
  }

  FreePool (SectorBuf);

  // ----- Step 4: Try El Torito path -----
  if (CatalogLba != 0) {
    Print (L"Parsing El Torito catalog...\r\n");
    Status = CayaParseElToritoCatalog (
               BlockIo,
               CatalogLba,
               &ImageLba,
               &ImageSectorCount,
               &PlatformId
               );

    if (!EFI_ERROR (Status)) {
      // Determine image size: El Torito SectorCount is in 512-byte units.
      // If 0, use a safe default of 2 MiB.
      if (ImageSectorCount == 0) {
        ImageLength = 2 * 1024 * 1024;
      } else {
        ImageLength = (UINT32)ImageSectorCount * 512;
      }

      // Round up to ISO sector boundary for the actual read
      UINT32 ReadSectors = (ImageLength + ISO9660_SECTOR_SIZE - 1) / ISO9660_SECTOR_SIZE;
      ImageData = AllocatePool ((UINTN)ReadSectors * ISO9660_SECTOR_SIZE);
      if (ImageData == NULL) {
        Print (L"Out of memory loading boot image.\r\n");
        CayaWaitForAnyKey ();
        return EFI_OUT_OF_RESOURCES;
      }

      Print (L"Loading El Torito image from LBA %u (%u bytes, platform 0x%02X)...\r\n",
             ImageLba, ImageLength, (UINT32)PlatformId);

      Status = CayaReadIsoSectors (BlockIo, ImageLba, ReadSectors, ImageData);
      if (!EFI_ERROR (Status)) {
        Status = CayaLoadAndStartImageFromBuffer (ParentImageHandle, ImageData, ImageLength);
      }

      FreePool (ImageData);

      if (!EFI_ERROR (Status)) {
        return EFI_SUCCESS;
      }

      Print (L"El Torito boot failed (%r), trying /EFI/BOOT/BOOTX64.EFI fallback...\r\n", Status);
    } else {
      Print (L"No bootable El Torito entry found, trying /EFI/BOOT/BOOTX64.EFI...\r\n");
    }
  } else {
    Print (L"No El Torito boot record found, trying /EFI/BOOT/BOOTX64.EFI...\r\n");
  }

  // ----- Step 5: Fallback – locate /EFI/BOOT/BOOTX64.EFI in ISO directory tree -----
  {
    UINT32 EfiFileLba, EfiFileLen;

    Print (L"Scanning ISO directory tree for /EFI/BOOT/BOOTX64.EFI...\r\n");
    Status = CayaFindUefiEfiInIso (BlockIo, &PvdCopy, &EfiFileLba, &EfiFileLen);
    if (EFI_ERROR (Status)) {
      Print (L"BOOTX64.EFI not found in ISO: ");
      CayaPrintStatus (Status);
      Print (L"\r\nThis ISO may not be UEFI bootable.\r\n");
      Print (L"\r\nPress any key to return.\r\n");
      CayaWaitForAnyKey ();
      return Status;
    }

    UINT32 ReadSectors2 = (EfiFileLen + ISO9660_SECTOR_SIZE - 1) / ISO9660_SECTOR_SIZE;
    ImageData = AllocatePool ((UINTN)ReadSectors2 * ISO9660_SECTOR_SIZE);
    if (ImageData == NULL) {
      Print (L"Out of memory.\r\n");
      CayaWaitForAnyKey ();
      return EFI_OUT_OF_RESOURCES;
    }

    Print (L"Loading /EFI/BOOT/BOOTX64.EFI from LBA %u (%u bytes)...\r\n",
           EfiFileLba, EfiFileLen);

    Status = CayaReadIsoSectors (BlockIo, EfiFileLba, ReadSectors2, ImageData);
    if (EFI_ERROR (Status)) {
      Print (L"Read failed: ");
      CayaPrintStatus (Status);
      FreePool (ImageData);
      CayaWaitForAnyKey ();
      return Status;
    }

    Status = CayaLoadAndStartImageFromBuffer (ParentImageHandle, ImageData, EfiFileLen);
    FreePool (ImageData);
  }

  if (EFI_ERROR (Status)) {
    Print (L"\r\nISO boot failed: ");
    CayaPrintStatus (Status);
    Print (L"\r\nPress any key to return.\r\n");
    CayaWaitForAnyKey ();
  }

  return Status;
}
