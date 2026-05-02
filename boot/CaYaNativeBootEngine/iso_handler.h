#ifndef CAYA_ISO_HANDLER_H
#define CAYA_ISO_HANDLER_H

#include <Uefi.h>
#include "file_scan.h"

// -----------------------------------------------------------------------
// ISO 9660 / El Torito on-disk structures (all fields little-endian)
// -----------------------------------------------------------------------

#pragma pack(1)

// Generic Volume Descriptor header (first 8 bytes are the same for all VDs)
typedef struct {
  UINT8  TypeCode;         // 0=Boot Record, 1=PVD, 2=SVD, 255=Terminator
  UINT8  Identifier[5];   // "CD001"
  UINT8  Version;
  UINT8  Data[2041];      // remainder varies by type
} ISO9660_VD_HEADER;

// Primary Volume Descriptor (Type 1) – fields we actually use
typedef struct {
  UINT8  TypeCode;
  UINT8  Identifier[5];
  UINT8  Version;
  UINT8  Unused1;
  UINT8  SystemIdentifier[32];
  UINT8  VolumeIdentifier[32];
  UINT8  Unused2[8];
  UINT32 VolumeSpaceSizeLe;
  UINT32 VolumeSpaceSizeBe;
  UINT8  Unused3[32];
  UINT16 VolumeSetSizeLe;
  UINT16 VolumeSetSizeBe;
  UINT16 VolumeSequenceNumberLe;
  UINT16 VolumeSequenceNumberBe;
  UINT16 LogicalBlockSizeLe;
  UINT16 LogicalBlockSizeBe;
  UINT32 PathTableSizeLe;
  UINT32 PathTableSizeBe;
  UINT32 PathTableLbaLe;
  UINT32 OptPathTableLbaLe;
  UINT32 PathTableLbaBe;
  UINT32 OptPathTableLbaBe;
  UINT8  RootDirEntry[34];
  UINT8  Padding[1858];
} ISO9660_PVD;

// Boot Record Volume Descriptor (Type 0) – El Torito
typedef struct {
  UINT8  TypeCode;           // 0
  UINT8  Identifier[5];     // "CD001"
  UINT8  Version;
  UINT8  BootSystemIdentifier[32]; // "EL TORITO SPECIFICATION"
  UINT8  Unused[32];
  UINT32 BootCatalogLba;    // LBA of El Torito boot catalog
  UINT8  Padding[1973];
} ISO9660_BOOT_RECORD_VD;

// El Torito Validation Entry (first entry in catalog)
typedef struct {
  UINT8  HeaderId;           // 0x01
  UINT8  PlatformId;         // 0=x86, 0xEF=UEFI, 0xAA=Mac EFI
  UINT16 Reserved;
  UINT8  ManufacturerString[24];
  UINT16 Checksum;
  UINT8  Key55;
  UINT8  KeyAA;
} ELTORITO_VALIDATION_ENTRY;

// El Torito Initial/Default Entry (second entry in catalog)
typedef struct {
  UINT8  Bootable;          // 0x88 = bootable
  UINT8  MediaType;         // 0=no emulation, 1=1.2M floppy, etc.
  UINT16 LoadSegment;
  UINT8  SystemType;
  UINT8  Unused1;
  UINT16 SectorCount;
  UINT32 LoadRba;           // LBA of boot image
  UINT8  Unused2[20];
} ELTORITO_DEFAULT_ENTRY;

// El Torito Section Header Entry
typedef struct {
  UINT8  HeaderIndicator; // 0x90 = more sections follow, 0x91 = last section
  UINT8  PlatformId;
  UINT16 NumEntries;
  UINT8  IdString[28];
} ELTORITO_SECTION_HEADER;

// El Torito Section Entry (same layout as Default Entry)
typedef struct {
  UINT8  Bootable;
  UINT8  MediaType;
  UINT16 LoadSegment;
  UINT8  SystemType;
  UINT8  Unused1;
  UINT16 SectorCount;
  UINT32 LoadRba;
  UINT8  SelectionCriteria;
  UINT8  VendorSpecific[19];
} ELTORITO_SECTION_ENTRY;

// ISO 9660 Directory Record (variable length – we only access the fixed part)
typedef struct {
  UINT8  Length;
  UINT8  ExtendedAttributeRecordLength;
  UINT32 ExtentLbaLe;
  UINT32 ExtentLbaBe;
  UINT32 DataLengthLe;
  UINT32 DataLengthBe;
  UINT8  RecordingDateTime[7];
  UINT8  FileFlags;
  UINT8  FileUnitSize;
  UINT8  InterleaveGapSize;
  UINT16 VolumeSequenceNumberLe;
  UINT16 VolumeSequenceNumberBe;
  UINT8  FileIdentifierLength;
  // UINT8 FileIdentifier[FileIdentifierLength] follows
} ISO9660_DIR_RECORD;

#pragma pack()

#define ISO9660_SECTOR_SIZE     2048
#define ISO9660_SYSTEM_AREA_LBA 16

#define ELTORITO_PLATFORM_UEFI  0xEF
#define ELTORITO_PLATFORM_X86   0x00
#define ELTORITO_BOOTABLE       0x88

// -----------------------------------------------------------------------
// Public API
// -----------------------------------------------------------------------

EFI_STATUS
CayaHandleIsoOrImage (
  IN EFI_HANDLE          ParentImageHandle,
  IN CAYA_FILE_SCAN_CONTEXT *ScanContext,
  IN CAYA_BOOT_ENTRY     *Entry
  );

#endif

