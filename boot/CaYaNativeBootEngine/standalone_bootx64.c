typedef unsigned char BOOLEAN;
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef unsigned long long UINT64;
typedef unsigned long long UINTN;
typedef unsigned long long EFI_STATUS;
typedef void *EFI_HANDLE;
typedef void *EFI_EVENT;
typedef wchar_t CHAR16;

#ifdef _MSC_VER
#pragma warning(disable:4505)
#endif

#define EFIERR(a) (0x8000000000000000ULL | (a))
#define EFI_SUCCESS 0
#define EFI_LOAD_ERROR EFIERR(1)
#define EFI_INVALID_PARAMETER EFIERR(2)
#define EFI_UNSUPPORTED EFIERR(3)
#define EFI_BAD_BUFFER_SIZE EFIERR(4)
#define EFI_BUFFER_TOO_SMALL EFIERR(5)
#define EFI_NOT_READY EFIERR(6)
#define EFI_WRITE_PROTECTED EFIERR(8)
#define EFI_NOT_FOUND EFIERR(14)
#define EFI_OUT_OF_RESOURCES EFIERR(9)
#define EFI_ERROR(Status) (((Status) & 0x8000000000000000ULL) != 0)

#define EFI_FILE_MODE_READ 0x0000000000000001ULL
#define EFI_FILE_MODE_WRITE 0x0000000000000002ULL
#define EFI_FILE_MODE_CREATE 0x8000000000000000ULL
#define EFI_FILE_DIRECTORY 0x0000000000000010ULL
#define EFI_LOADER_DATA 2
#define MAX_ENTRIES 96
#define MAX_VOLUME_ROOTS 32
#define MAX_NAME 128
#define MAX_PATH 260
#define MAX_EFI_LOAD_SIZE (64ULL * 1024ULL * 1024ULL)

#define SCAN_UP 0x0001
#define SCAN_DOWN 0x0002
#define SCAN_ESC 0x0017
#define CHAR_CARRIAGE_RETURN 0x000D
#define MENU_VISIBLE_ROWS 10

#define EFI_BLACK 0x00
#define EFI_BLUE 0x01
#define EFI_GREEN 0x02
#define EFI_CYAN 0x03
#define EFI_RED 0x04
#define EFI_MAGENTA 0x05
#define EFI_BROWN 0x06
#define EFI_LIGHTGRAY 0x07
#define EFI_DARKGRAY 0x08
#define EFI_LIGHTBLUE 0x09
#define EFI_LIGHTGREEN 0x0A
#define EFI_LIGHTCYAN 0x0B
#define EFI_LIGHTRED 0x0C
#define EFI_LIGHTMAGENTA 0x0D
#define EFI_YELLOW 0x0E
#define EFI_WHITE 0x0F
#define EFI_TEXT_ATTR(Fg, Bg) ((Fg) | ((Bg) << 4))

/* Text-mode approximation of the CaYaDev site palette: black canvas,
   white/gray text, and CaYa red accents. */
#define UI_BG EFI_BLACK
#define UI_TEXT EFI_WHITE
#define UI_MUTED EFI_LIGHTGRAY
#define UI_DIM EFI_DARKGRAY
#define UI_ACCENT EFI_LIGHTRED
#define UI_SELECTED_BG EFI_RED

typedef struct {
    UINT32 Data1;
    UINT16 Data2;
    UINT16 Data3;
    UINT8 Data4[8];
} EFI_GUID;

typedef struct {
    UINT16 Year;
    UINT8 Month;
    UINT8 Day;
    UINT8 Hour;
    UINT8 Minute;
    UINT8 Second;
    UINT8 Pad1;
    UINT32 Nanosecond;
    short TimeZone;
    UINT8 Daylight;
    UINT8 Pad2;
} EFI_TIME;

typedef struct {
    UINT64 Signature;
    UINT32 Revision;
    UINT32 HeaderSize;
    UINT32 CRC32;
    UINT32 Reserved;
} EFI_TABLE_HEADER;

typedef struct {
    UINT16 ScanCode;
    CHAR16 UnicodeChar;
} EFI_INPUT_KEY;

typedef struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL EFI_SIMPLE_TEXT_INPUT_PROTOCOL;
typedef EFI_STATUS (*EFI_INPUT_RESET)(EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This, BOOLEAN ExtendedVerification);
typedef EFI_STATUS (*EFI_INPUT_READ_KEY)(EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This, EFI_INPUT_KEY *Key);
struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL {
    EFI_INPUT_RESET Reset;
    EFI_INPUT_READ_KEY ReadKeyStroke;
    EFI_EVENT WaitForKey;
};

typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;
typedef EFI_STATUS (*EFI_TEXT_RESET)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, BOOLEAN ExtendedVerification);
typedef EFI_STATUS (*EFI_TEXT_STRING)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, CHAR16 *String);
typedef EFI_STATUS (*EFI_TEXT_CLEAR)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This);
typedef EFI_STATUS (*EFI_TEXT_SET_ATTRIBUTE)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, UINTN Attribute);
typedef EFI_STATUS (*EFI_TEXT_SET_CURSOR_POSITION)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, UINTN Column, UINTN Row);
typedef EFI_STATUS (*EFI_TEXT_ENABLE_CURSOR)(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, BOOLEAN Visible);
struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
    EFI_TEXT_RESET Reset;
    EFI_TEXT_STRING OutputString;
    void *TestString;
    void *QueryMode;
    void *SetMode;
    EFI_TEXT_SET_ATTRIBUTE SetAttribute;
    EFI_TEXT_CLEAR ClearScreen;
    EFI_TEXT_SET_CURSOR_POSITION SetCursorPosition;
    EFI_TEXT_ENABLE_CURSOR EnableCursor;
};

typedef enum {
    EfiResetCold,
    EfiResetWarm,
    EfiResetShutdown,
    EfiResetPlatformSpecific
} EFI_RESET_TYPE;

typedef struct _EFI_RUNTIME_SERVICES EFI_RUNTIME_SERVICES;
typedef void (*EFI_RESET_SYSTEM)(EFI_RESET_TYPE ResetType, EFI_STATUS ResetStatus, UINTN DataSize, void *ResetData);
struct _EFI_RUNTIME_SERVICES {
    EFI_TABLE_HEADER Hdr;
    void *GetTime;
    void *SetTime;
    void *GetWakeupTime;
    void *SetWakeupTime;
    void *SetVirtualAddressMap;
    void *ConvertPointer;
    void *GetVariable;
    void *GetNextVariableName;
    void *SetVariable;
    void *GetNextHighMonotonicCount;
    EFI_RESET_SYSTEM ResetSystem;
};

typedef struct _EFI_BOOT_SERVICES EFI_BOOT_SERVICES;
typedef enum {
    AllHandles,
    ByRegisterNotify,
    ByProtocol
} EFI_LOCATE_SEARCH_TYPE;
typedef EFI_STATUS (*EFI_ALLOCATE_POOL)(UINT32 PoolType, UINTN Size, void **Buffer);
typedef EFI_STATUS (*EFI_FREE_POOL)(void *Buffer);
typedef EFI_STATUS (*EFI_WAIT_FOR_EVENT)(UINTN NumberOfEvents, EFI_EVENT *Event, UINTN *Index);
typedef EFI_STATUS (*EFI_HANDLE_PROTOCOL)(EFI_HANDLE Handle, EFI_GUID *Protocol, void **Interface);
typedef EFI_STATUS (*EFI_LOCATE_HANDLE)(EFI_LOCATE_SEARCH_TYPE SearchType, EFI_GUID *Protocol, void *SearchKey, UINTN *BufferSize, EFI_HANDLE *Buffer);
typedef EFI_STATUS (*EFI_LOAD_IMAGE)(BOOLEAN BootPolicy, EFI_HANDLE ParentImageHandle, void *DevicePath, void *SourceBuffer, UINTN SourceSize, EFI_HANDLE *ImageHandle);
typedef EFI_STATUS (*EFI_START_IMAGE)(EFI_HANDLE ImageHandle, UINTN *ExitDataSize, CHAR16 **ExitData);
typedef EFI_STATUS (*EFI_CONNECT_CONTROLLER)(EFI_HANDLE ControllerHandle, EFI_HANDLE *DriverImageHandle, void *RemainingDevicePath, BOOLEAN Recursive);
typedef EFI_STATUS (*EFI_INSTALL_PROTOCOL_INTERFACE)(EFI_HANDLE *Handle, EFI_GUID *Protocol, UINT32 InterfaceType, void *Interface);
struct _EFI_BOOT_SERVICES {
    EFI_TABLE_HEADER Hdr;
    void *RaiseTPL;
    void *RestoreTPL;
    void *AllocatePages;
    void *FreePages;
    void *GetMemoryMap;
    EFI_ALLOCATE_POOL AllocatePool;
    EFI_FREE_POOL FreePool;
    void *CreateEvent;
    void *SetTimer;
    EFI_WAIT_FOR_EVENT WaitForEvent;
    void *SignalEvent;
    void *CloseEvent;
    void *CheckEvent;
    EFI_INSTALL_PROTOCOL_INTERFACE InstallProtocolInterface;
    void *ReinstallProtocolInterface;
    void *UninstallProtocolInterface;
    EFI_HANDLE_PROTOCOL HandleProtocol;
    void *Reserved;
    void *RegisterProtocolNotify;
    EFI_LOCATE_HANDLE LocateHandle;
    void *LocateDevicePath;
    void *InstallConfigurationTable;
    EFI_LOAD_IMAGE LoadImage;
    EFI_START_IMAGE StartImage;
    void *Exit;
    void *UnloadImage;
    void *ExitBootServices;
    void *GetNextMonotonicCount;
    void *Stall;
    void *SetWatchdogTimer;
    EFI_CONNECT_CONTROLLER ConnectController;
};

typedef struct {
    EFI_TABLE_HEADER Hdr;
    CHAR16 *FirmwareVendor;
    UINT32 FirmwareRevision;
    EFI_HANDLE ConsoleInHandle;
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL *ConIn;
    EFI_HANDLE ConsoleOutHandle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut;
    EFI_HANDLE StandardErrorHandle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *StdErr;
    EFI_RUNTIME_SERVICES *RuntimeServices;
    EFI_BOOT_SERVICES *BootServices;
} EFI_SYSTEM_TABLE;

typedef struct {
    UINT8 Type;
    UINT8 SubType;
    UINT8 Length[2];
} EFI_DEVICE_PATH_PROTOCOL;

typedef struct {
    UINT32 Revision;
    EFI_HANDLE ParentHandle;
    EFI_SYSTEM_TABLE *SystemTable;
    EFI_HANDLE DeviceHandle;
    EFI_DEVICE_PATH_PROTOCOL *FilePath;
    void *Reserved;
    UINT32 LoadOptionsSize;
    void *LoadOptions;
    void *ImageBase;
    UINT64 ImageSize;
    UINT32 ImageCodeType;
    UINT32 ImageDataType;
    EFI_STATUS (*Unload)(EFI_HANDLE ImageHandle);
} EFI_LOADED_IMAGE_PROTOCOL;

/* ---- EFI Block I/O ---- */
typedef struct {
    UINT32  MediaId;
    BOOLEAN RemovableMedia;
    BOOLEAN MediaPresent;
    BOOLEAN LogicalPartition;
    BOOLEAN ReadOnly;
    BOOLEAN WriteCaching;
    UINT32  BlockSize;
    UINT32  IoAlign;
    UINT64  LastBlock;
} EFI_BLOCK_IO_MEDIA;

typedef struct _EFI_BLOCK_IO_PROTOCOL EFI_BLOCK_IO_PROTOCOL;
typedef EFI_STATUS (*EFI_BLOCK_READ)(EFI_BLOCK_IO_PROTOCOL *This, UINT32 MediaId, UINT64 Lba, UINTN BufferSize, void *Buffer);
typedef EFI_STATUS (*EFI_BLOCK_RESET)(EFI_BLOCK_IO_PROTOCOL *This, BOOLEAN ExtendedVerification);
typedef EFI_STATUS (*EFI_BLOCK_WRITE)(EFI_BLOCK_IO_PROTOCOL *This, UINT32 MediaId, UINT64 Lba, UINTN BufferSize, void *Buffer);
typedef EFI_STATUS (*EFI_BLOCK_FLUSH)(EFI_BLOCK_IO_PROTOCOL *This);
struct _EFI_BLOCK_IO_PROTOCOL {
    UINT64              Revision;
    EFI_BLOCK_IO_MEDIA *Media;
    EFI_BLOCK_RESET     Reset;
    EFI_BLOCK_READ      ReadBlocks;
    EFI_BLOCK_WRITE     WriteBlocks;
    EFI_BLOCK_FLUSH     FlushBlocks;
};

typedef struct _EFI_DISK_IO_PROTOCOL EFI_DISK_IO_PROTOCOL;
typedef EFI_STATUS (*EFI_DISK_READ)(EFI_DISK_IO_PROTOCOL *This, UINT32 MediaId, UINT64 Offset, UINTN BufferSize, void *Buffer);
typedef EFI_STATUS (*EFI_DISK_WRITE)(EFI_DISK_IO_PROTOCOL *This, UINT32 MediaId, UINT64 Offset, UINTN BufferSize, void *Buffer);
struct _EFI_DISK_IO_PROTOCOL {
    UINT64         Revision;
    EFI_DISK_READ  ReadDisk;
    EFI_DISK_WRITE WriteDisk;
};

typedef struct _EFI_FILE_PROTOCOL EFI_FILE_PROTOCOL;
typedef EFI_STATUS (*EFI_FILE_OPEN)(EFI_FILE_PROTOCOL *This, EFI_FILE_PROTOCOL **NewHandle, CHAR16 *FileName, UINT64 OpenMode, UINT64 Attributes);
typedef EFI_STATUS (*EFI_FILE_CLOSE)(EFI_FILE_PROTOCOL *This);
typedef EFI_STATUS (*EFI_FILE_READ)(EFI_FILE_PROTOCOL *This, UINTN *BufferSize, void *Buffer);
typedef EFI_STATUS (*EFI_FILE_WRITE)(EFI_FILE_PROTOCOL *This, UINTN *BufferSize, void *Buffer);
typedef EFI_STATUS (*EFI_FILE_GET_POSITION)(EFI_FILE_PROTOCOL *This, UINT64 *Position);
typedef EFI_STATUS (*EFI_FILE_SET_POSITION)(EFI_FILE_PROTOCOL *This, UINT64 Position);
typedef EFI_STATUS (*EFI_FILE_GET_INFO)(EFI_FILE_PROTOCOL *This, EFI_GUID *InformationType, UINTN *BufferSize, void *Buffer);
struct _EFI_FILE_PROTOCOL {
    UINT64 Revision;
    EFI_FILE_OPEN Open;
    EFI_FILE_CLOSE Close;
    void *Delete;
    EFI_FILE_READ Read;
    EFI_FILE_WRITE Write;
    EFI_FILE_GET_POSITION GetPosition;
    EFI_FILE_SET_POSITION SetPosition;
    EFI_FILE_GET_INFO GetInfo;
};

typedef struct {
    UINT64 Revision;
    EFI_STATUS (*OpenVolume)(void *This, EFI_FILE_PROTOCOL **Root);
} EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

typedef struct {
    UINT8 Blue;
    UINT8 Green;
    UINT8 Red;
    UINT8 Reserved;
} EFI_GRAPHICS_OUTPUT_BLT_PIXEL;

typedef enum {
    EfiBltVideoFill,
    EfiBltVideoToBltBuffer,
    EfiBltBufferToVideo,
    EfiBltVideoToVideo
} EFI_GRAPHICS_OUTPUT_BLT_OPERATION;

typedef struct {
    UINT32 Version;
    UINT32 HorizontalResolution;
    UINT32 VerticalResolution;
    UINT32 PixelFormat;
    void *PixelInformation;
    UINT32 PixelsPerScanLine;
} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;

typedef struct {
    UINT32 MaxMode;
    UINT32 Mode;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
    UINTN SizeOfInfo;
    UINT64 FrameBufferBase;
    UINTN FrameBufferSize;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

typedef struct _EFI_GRAPHICS_OUTPUT_PROTOCOL EFI_GRAPHICS_OUTPUT_PROTOCOL;
typedef EFI_STATUS (*EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT)(
    EFI_GRAPHICS_OUTPUT_PROTOCOL *This,
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer,
    EFI_GRAPHICS_OUTPUT_BLT_OPERATION BltOperation,
    UINTN SourceX,
    UINTN SourceY,
    UINTN DestinationX,
    UINTN DestinationY,
    UINTN Width,
    UINTN Height,
    UINTN Delta);

struct _EFI_GRAPHICS_OUTPUT_PROTOCOL {
    void *QueryMode;
    void *SetMode;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT Blt;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *Mode;
};

typedef struct {
    UINT64 Size;
    UINT64 FileSize;
    UINT64 PhysicalSize;
    EFI_TIME CreateTime;
    EFI_TIME LastAccessTime;
    EFI_TIME ModificationTime;
    UINT64 Attribute;
    CHAR16 FileName[1];
} EFI_FILE_INFO;

typedef enum {
    BootFileIso,
    BootFileImg,
    BootFileEfi
} BOOT_FILE_TYPE;

/* ---- exFAT read-only Block I/O support ---- */
#define EXFAT_FAT_EOC        0xFFFFFFF8u
#define EXFAT_ENTRY_FILE     0x85u
#define EXFAT_ENTRY_STREAM   0xC0u
#define EXFAT_ENTRY_NAME     0xC1u
#define EXFAT_ATTR_DIRECTORY 0x10u
#define MAX_EXFAT_VOLS       4

typedef struct {
    EFI_BLOCK_IO_PROTOCOL *Bio;
    UINT32 MediaId;
    UINT32 BytesPerSector;
    UINT32 BytesPerCluster;
    UINT64 FatLba;       /* absolute LBA of FAT start */
    UINT64 HeapLba;      /* absolute LBA of cluster 2 (heap start) */
    UINT32 RootCluster;  /* first cluster of root directory */
} EXFAT_VOL;

typedef struct {
    EFI_FILE_PROTOCOL *Root;       /* non-NULL: FAT SimpleFileSystem volume */
    EXFAT_VOL         *ExVol;      /* non-NULL: exFAT Block I/O volume */
    UINT32             ExCluster;  /* first data cluster (exFAT only) */
    UINT8              ExNoFatChain; /* 1 = contiguous clusters, skip FAT (exFAT only) */
    CHAR16 Name[MAX_NAME];
    CHAR16 Path[MAX_PATH];
    UINT64 Size;
    BOOT_FILE_TYPE Type;
} BOOT_ENTRY;

typedef struct {
    BOOT_ENTRY Entries[MAX_ENTRIES];
    EFI_FILE_PROTOCOL *OpenedRoots[MAX_VOLUME_ROOTS];
    UINTN OpenedRootCount;
    EXFAT_VOL ExfatVols[MAX_EXFAT_VOLS];
    UINTN ExfatVolCount;
    UINTN Count;
    BOOLEAN Truncated;
} BOOT_LIST;

static EFI_GUID LoadedImageProtocolGuid = {0x5B1B31A1, 0x9562, 0x11D2, {0x8E, 0x3F, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B}};
static EFI_GUID SimpleFileSystemProtocolGuid = {0x964E5B22, 0x6459, 0x11D2, {0x8E, 0x39, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B}};
static EFI_GUID GraphicsOutputProtocolGuid = {0x9042A9DE, 0x23DC, 0x4A38, {0x96, 0xFB, 0x7A, 0xDE, 0xD0, 0x80, 0x51, 0x6A}};
static EFI_GUID BlockIoProtocolGuid = {0x964E5B21, 0x6459, 0x11D2, {0x8E, 0x39, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B}};
static EFI_GUID DiskIoProtocolGuid = {0xCE345171, 0xBA0B, 0x11D2, {0x8E, 0x4F, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B}};
static EFI_GUID DevicePathProtocolGuid = {0x09576E91, 0x6D3F, 0x11D2, {0x8E, 0x39, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B}};
static EFI_GUID FileInfoGuid = {0x09576E92, 0x6D3F, 0x11D2, {0x8E, 0x39, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B}};
static EFI_GUID CayaRamDiskVendorGuid = {0xCA7A150B, 0xB007, 0x4F10, {0x91, 0xC0, 0xCA, 0x7A, 0x15, 0x0B, 0x00, 0x01}};

static EFI_SYSTEM_TABLE *gST;
static EFI_BOOT_SERVICES *gBS;
static EFI_HANDLE gImageHandle;
static EFI_HANDLE gDeviceHandle;
static EFI_FILE_PROTOCOL *gRoot;
static EFI_GRAPHICS_OUTPUT_PROTOCOL *gGop;
static const CHAR16 *gIsoFailureDetail;

#define CAYA_LOGO_SOURCE_WIDTH 168
#define CAYA_LOGO_SOURCE_HEIGHT 22
#define CAYA_LOGO_COLOR_RED 1
#define CAYA_LOGO_COLOR_WHITE 2

typedef struct {
    UINT8 Y;
    UINT16 X;
    UINT16 Width;
    UINT8 Color;
} CAYA_LOGO_RUN;

/* Rasterized from src/CaYaISOBoot.App/Assets/CaYaDev.svg for UEFI GOP.
   The source SVG is not parsed at boot; this keeps BOOTX64.EFI standalone. */
static const CAYA_LOGO_RUN kCayaDevLogoRuns[] = {
    {0, 0, 27, 1},
    {0, 33, 13, 1},
    {0, 49, 5, 1},
    {0, 73, 5, 1},
    {0, 81, 12, 1},
    {0, 97, 6, 2},
    {0, 104, 1, 2},
    {0, 123, 17, 2},
    {0, 141, 4, 2},
    {0, 165, 3, 2},
    {1, 0, 26, 1},
    {1, 33, 13, 1},
    {1, 50, 5, 1},
    {1, 72, 5, 1},
    {1, 81, 12, 1},
    {1, 98, 5, 2},
    {1, 104, 3, 2},
    {1, 123, 17, 2},
    {1, 142, 4, 2},
    {1, 164, 4, 2},
    {2, 0, 25, 1},
    {2, 33, 13, 1},
    {2, 51, 5, 1},
    {2, 71, 5, 1},
    {2, 80, 14, 1},
    {2, 98, 5, 2},
    {2, 104, 6, 2},
    {2, 123, 15, 2},
    {2, 142, 5, 2},
    {2, 163, 4, 2},
    {3, 0, 23, 1},
    {3, 33, 14, 1},
    {3, 52, 5, 1},
    {3, 70, 5, 1},
    {3, 80, 14, 1},
    {3, 98, 5, 2},
    {3, 105, 7, 2},
    {3, 123, 9, 2},
    {3, 143, 5, 2},
    {3, 163, 4, 2},
    {4, 0, 22, 1},
    {4, 32, 15, 1},
    {4, 53, 5, 1},
    {4, 69, 6, 1},
    {4, 80, 15, 1},
    {4, 99, 5, 2},
    {4, 107, 7, 2},
    {4, 124, 5, 2},
    {4, 144, 5, 2},
    {4, 162, 4, 2},
    {5, 0, 20, 1},
    {5, 32, 15, 1},
    {5, 54, 6, 1},
    {5, 68, 6, 1},
    {5, 80, 15, 1},
    {5, 99, 5, 2},
    {5, 109, 7, 2},
    {5, 124, 5, 2},
    {5, 144, 6, 2},
    {5, 161, 5, 2},
    {6, 0, 5, 1},
    {6, 32, 16, 1},
    {6, 54, 7, 1},
    {6, 67, 6, 1},
    {6, 79, 16, 1},
    {6, 99, 5, 2},
    {6, 111, 7, 2},
    {6, 124, 6, 2},
    {6, 145, 6, 2},
    {6, 161, 4, 2},
    {7, 0, 5, 1},
    {7, 32, 7, 1},
    {7, 41, 7, 1},
    {7, 55, 7, 1},
    {7, 66, 6, 1},
    {7, 79, 8, 1},
    {7, 88, 8, 1},
    {7, 99, 5, 2},
    {7, 112, 8, 2},
    {7, 125, 5, 2},
    {7, 146, 6, 2},
    {7, 160, 5, 2},
    {8, 0, 5, 1},
    {8, 31, 8, 1},
    {8, 41, 8, 1},
    {8, 56, 7, 1},
    {8, 65, 6, 1},
    {8, 79, 7, 1},
    {8, 89, 7, 1},
    {8, 100, 5, 2},
    {8, 114, 8, 2},
    {8, 125, 5, 2},
    {8, 146, 7, 2},
    {8, 160, 4, 2},
    {9, 0, 5, 1},
    {9, 31, 7, 1},
    {9, 42, 7, 1},
    {9, 57, 14, 1},
    {9, 79, 7, 1},
    {9, 89, 7, 1},
    {9, 100, 5, 2},
    {9, 116, 8, 2},
    {9, 125, 14, 2},
    {9, 147, 7, 2},
    {9, 159, 5, 2},
    {10, 0, 5, 1},
    {10, 31, 7, 1},
    {10, 43, 6, 1},
    {10, 58, 12, 1},
    {10, 78, 7, 1},
    {10, 90, 7, 1},
    {10, 100, 5, 2},
    {10, 117, 7, 2},
    {10, 126, 13, 2},
    {10, 148, 7, 2},
    {10, 158, 6, 2},
    {11, 0, 5, 1},
    {11, 31, 8, 1},
    {11, 41, 9, 1},
    {11, 59, 10, 1},
    {11, 78, 8, 1},
    {11, 89, 8, 1},
    {11, 101, 5, 2},
    {11, 116, 7, 2},
    {11, 126, 13, 2},
    {11, 149, 7, 2},
    {11, 158, 5, 2},
    {12, 0, 5, 1},
    {12, 30, 20, 1},
    {12, 60, 8, 1},
    {12, 78, 20, 1},
    {12, 101, 5, 2},
    {12, 115, 7, 2},
    {12, 126, 9, 2},
    {12, 149, 7, 2},
    {12, 157, 6, 2},
    {13, 0, 5, 1},
    {13, 30, 20, 1},
    {13, 61, 7, 1},
    {13, 78, 20, 1},
    {13, 101, 5, 2},
    {13, 114, 6, 2},
    {13, 127, 4, 2},
    {13, 150, 12, 2},
    {14, 0, 5, 1},
    {14, 30, 14, 1},
    {14, 45, 6, 1},
    {14, 61, 6, 1},
    {14, 77, 15, 1},
    {14, 93, 5, 1},
    {14, 102, 4, 2},
    {14, 113, 6, 2},
    {14, 127, 5, 2},
    {14, 151, 11, 2},
    {15, 0, 5, 1},
    {15, 30, 5, 1},
    {15, 38, 5, 1},
    {15, 46, 5, 1},
    {15, 61, 6, 1},
    {15, 77, 5, 1},
    {15, 85, 5, 1},
    {15, 93, 6, 1},
    {15, 102, 5, 2},
    {15, 112, 6, 2},
    {15, 127, 5, 2},
    {15, 151, 10, 2},
    {16, 0, 5, 1},
    {16, 30, 4, 1},
    {16, 47, 5, 1},
    {16, 61, 6, 1},
    {16, 77, 5, 1},
    {16, 94, 5, 1},
    {16, 102, 5, 2},
    {16, 111, 5, 2},
    {16, 128, 4, 2},
    {16, 152, 9, 2},
    {17, 0, 20, 1},
    {17, 29, 5, 1},
    {17, 47, 5, 1},
    {17, 62, 5, 1},
    {17, 77, 4, 1},
    {17, 95, 4, 1},
    {17, 103, 4, 2},
    {17, 110, 5, 2},
    {17, 128, 4, 2},
    {17, 153, 7, 2},
    {18, 0, 22, 1},
    {18, 29, 4, 1},
    {18, 48, 4, 1},
    {18, 62, 5, 1},
    {18, 76, 5, 1},
    {18, 95, 5, 1},
    {18, 103, 4, 2},
    {18, 109, 5, 2},
    {18, 128, 8, 2},
    {18, 153, 7, 2},
    {19, 0, 24, 1},
    {19, 29, 4, 1},
    {19, 49, 4, 1},
    {19, 62, 5, 1},
    {19, 76, 4, 1},
    {19, 96, 4, 1},
    {19, 103, 9, 2},
    {19, 129, 12, 2},
    {19, 154, 5, 2},
    {20, 0, 26, 1},
    {20, 29, 3, 1},
    {20, 49, 4, 1},
    {20, 62, 5, 1},
    {20, 76, 4, 1},
    {20, 97, 4, 1},
    {20, 104, 7, 2},
    {20, 129, 13, 2},
    {20, 155, 4, 2},
    {21, 0, 27, 1},
    {21, 28, 4, 1},
    {21, 50, 3, 1},
    {21, 63, 3, 1},
    {21, 76, 3, 1},
    {21, 97, 4, 1},
    {21, 104, 6, 2},
    {21, 129, 14, 2},
    {21, 155, 3, 2},
};

static void Print(const CHAR16 *Text) {
    gST->ConOut->OutputString(gST->ConOut, (CHAR16 *)Text);
}

static void SetTextAttribute(UINTN Foreground, UINTN Background);

static void Clear(void) {
    /* Firmware default attribute is often blue background. Force the dark
       theme background BEFORE clearing so the entire console area is dark. */
    SetTextAttribute(UI_MUTED, UI_BG);
    gST->ConOut->ClearScreen(gST->ConOut);
}

static void SetTextAttribute(UINTN Foreground, UINTN Background) {
    if (gST->ConOut->SetAttribute != 0) {
        gST->ConOut->SetAttribute(gST->ConOut, EFI_TEXT_ATTR(Foreground, Background));
    }
}

static void SetCursor(UINTN Column, UINTN Row) {
    if (gST->ConOut->SetCursorPosition != 0) {
        gST->ConOut->SetCursorPosition(gST->ConOut, Column, Row);
    }
}

static void HideCursor(void) {
    if (gST->ConOut->EnableCursor != 0) {
        gST->ConOut->EnableCursor(gST->ConOut, 0);
    }
}

static EFI_GRAPHICS_OUTPUT_BLT_PIXEL Rgb(UINT8 Red, UINT8 Green, UINT8 Blue) {
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Pixel;
    Pixel.Red = Red;
    Pixel.Green = Green;
    Pixel.Blue = Blue;
    Pixel.Reserved = 0;
    return Pixel;
}

static UINTN MinU(UINTN Left, UINTN Right) {
    return Left < Right ? Left : Right;
}

static void FillRect(UINTN X, UINTN Y, UINTN Width, UINTN Height, EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color) {
    UINTN ScreenWidth;
    UINTN ScreenHeight;

    if (gGop == 0 || gGop->Mode == 0 || gGop->Mode->Info == 0 || Width == 0 || Height == 0) {
        return;
    }

    ScreenWidth = gGop->Mode->Info->HorizontalResolution;
    ScreenHeight = gGop->Mode->Info->VerticalResolution;
    if (X >= ScreenWidth || Y >= ScreenHeight) {
        return;
    }

    Width = MinU(Width, ScreenWidth - X);
    Height = MinU(Height, ScreenHeight - Y);
    gGop->Blt(gGop, &Color, EfiBltVideoFill, 0, 0, X, Y, Width, Height, 0);
}

static UINTN LogoScale(void) {
    if (gGop != 0 && gGop->Mode != 0 && gGop->Mode->Info != 0 &&
        gGop->Mode->Info->HorizontalResolution >= 1024 &&
        gGop->Mode->Info->VerticalResolution >= 600) {
        return 2;
    }
    return 1;
}

static void DrawCaYaDevLogo(UINTN X, UINTN Y) {
    UINTN Index;
    UINTN Scale;

    if (gGop == 0 || gGop->Blt == 0) {
        return;
    }

    Scale = LogoScale();
    for (Index = 0; Index < sizeof(kCayaDevLogoRuns) / sizeof(kCayaDevLogoRuns[0]); Index++) {
        const CAYA_LOGO_RUN *Run = &kCayaDevLogoRuns[Index];
        EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color =
            Run->Color == CAYA_LOGO_COLOR_RED ? Rgb(220, 39, 39) : Rgb(255, 255, 255);

        FillRect(X + ((UINTN)Run->X * Scale),
                 Y + ((UINTN)Run->Y * Scale),
                 (UINTN)Run->Width * Scale,
                 Scale,
                 Color);
    }
}

static void InitializeGraphics(void) {
    EFI_STATUS Status;
    UINTN BufferSize = 0;
    EFI_HANDLE *Handles = 0;
    UINTN HandleCount;
    UINTN Index;

    gGop = 0;
    Status = gBS->LocateHandle(ByProtocol, &GraphicsOutputProtocolGuid, 0, &BufferSize, 0);
    if (Status != EFI_BUFFER_TOO_SMALL || BufferSize == 0) {
        return;
    }

    Status = gBS->AllocatePool(EFI_LOADER_DATA, BufferSize, (void **)&Handles);
    if (EFI_ERROR(Status)) {
        return;
    }

    Status = gBS->LocateHandle(ByProtocol, &GraphicsOutputProtocolGuid, 0, &BufferSize, Handles);
    if (!EFI_ERROR(Status)) {
        HandleCount = BufferSize / sizeof(EFI_HANDLE);
        for (Index = 0; Index < HandleCount; Index++) {
            EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop = 0;
            Status = gBS->HandleProtocol(Handles[Index], &GraphicsOutputProtocolGuid, (void **)&Gop);
            if (!EFI_ERROR(Status) && Gop != 0 && Gop->Mode != 0 && Gop->Mode->Info != 0 && Gop->Blt != 0) {
                gGop = Gop;
                break;
            }
        }
    }

    gBS->FreePool(Handles);
}

static void DrawGraphicChrome(UINTN VisibleRows, UINTN SelectedVisibleRow) {
    UINTN ScreenWidth;
    UINTN ScreenHeight;

    (void)VisibleRows;
    (void)SelectedVisibleRow;

    if (gGop != 0 && gGop->Mode != 0 && gGop->Mode->Info != 0) {
        ScreenWidth = gGop->Mode->Info->HorizontalResolution;
        ScreenHeight = gGop->Mode->Info->VerticalResolution;

        /* UEFI text cells have an opaque background color. Keep the GOP
           canvas exactly black, matching the text attribute background, so
           firmware does not render visible blocks behind each glyph. */
        FillRect(0, 0, ScreenWidth, ScreenHeight, Rgb(0, 0, 0));
    }
}

static UINTN StrLen(const CHAR16 *Text) {
    UINTN Count = 0;
    while (Text[Count] != 0) {
        Count++;
    }
    return Count;
}

static CHAR16 Upper(CHAR16 Ch) {
    if (Ch >= L'a' && Ch <= L'z') {
        return (CHAR16)(Ch - (L'a' - L'A'));
    }
    return Ch;
}

static int StrCmp(const CHAR16 *Left, const CHAR16 *Right) {
    UINTN Index = 0;
    while (Left[Index] != 0 || Right[Index] != 0) {
        if (Left[Index] != Right[Index]) {
            return Left[Index] < Right[Index] ? -1 : 1;
        }
        Index++;
    }
    return 0;
}

static int StriCmp(const CHAR16 *Left, const CHAR16 *Right) {
    UINTN Index = 0;
    while (Left[Index] != 0 || Right[Index] != 0) {
        CHAR16 A = Upper(Left[Index]);
        CHAR16 B = Upper(Right[Index]);
        if (A != B) {
            return A < B ? -1 : 1;
        }
        Index++;
    }
    return 0;
}

static void CopyString(CHAR16 *Destination, UINTN DestinationChars, const CHAR16 *Source) {
    UINTN Index = 0;
    if (DestinationChars == 0) {
        return;
    }
    while (Index + 1 < DestinationChars && Source[Index] != 0) {
        Destination[Index] = Source[Index];
        Index++;
    }
    Destination[Index] = 0;
}

static void AppendString(CHAR16 *Destination, UINTN DestinationChars, const CHAR16 *Source) {
    UINTN Length = StrLen(Destination);
    UINTN Index = 0;
    while (Length + Index + 1 < DestinationChars && Source[Index] != 0) {
        Destination[Length + Index] = Source[Index];
        Index++;
    }
    Destination[Length + Index] = 0;
}

static void Zero(void *Buffer, UINTN Size) {
    UINT8 *Bytes = (UINT8 *)Buffer;
    UINTN Index;
    for (Index = 0; Index < Size; Index++) {
        Bytes[Index] = 0;
    }
}

static void CopyBytes(void *Destination, const void *Source, UINTN Size) {
    UINT8 *Dst = (UINT8 *)Destination;
    const UINT8 *Src = (const UINT8 *)Source;
    UINTN Index;
    for (Index = 0; Index < Size; Index++) {
        Dst[Index] = Src[Index];
    }
}

static void PrintUInt(UINT64 Value) {
    CHAR16 Buffer[32];
    UINTN Index = 31;
    Buffer[Index] = 0;
    if (Value == 0) {
        Print(L"0");
        return;
    }
    while (Value > 0 && Index > 0) {
        UINT64 Digit = Value % 10;
        Index--;
        Buffer[Index] = (CHAR16)(L'0' + Digit);
        Value = Value / 10;
    }
    Print(&Buffer[Index]);
}

static void PrintLimited(const CHAR16 *Text, UINTN MaxChars) {
    CHAR16 Buffer[96];
    UINTN Index;
    UINTN Limit = MaxChars;

    if (Limit >= 96) {
        Limit = 95;
    }
    if (Limit == 0) {
        return;
    }

    for (Index = 0; Index + 1 < Limit && Text[Index] != 0; Index++) {
        Buffer[Index] = Text[Index];
    }

    if (Text[Index] != 0 && Limit > 4) {
        Buffer[Index++] = L'.';
        Buffer[Index++] = L'.';
        Buffer[Index++] = L'.';
    }

    Buffer[Index] = 0;
    Print(Buffer);
}

static void PrintAt(UINTN Column, UINTN Row, const CHAR16 *Text) {
    SetCursor(Column, Row);
    Print(Text);
}

static EFI_STATUS IsoFail(EFI_STATUS Status, const CHAR16 *Message) {
    gIsoFailureDetail = Message;
    return Status;
}

static void WaitKey(void) {
    EFI_INPUT_KEY Key;
    UINTN Index;
    gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &Index);
    while (gST->ConIn->ReadKeyStroke(gST->ConIn, &Key) == EFI_NOT_READY) {
    }
}

static void Header(void) {
    if (gGop != 0) {
        DrawGraphicChrome(0, 0);
        DrawCaYaDevLogo(16, LogoScale() >= 2 ? 14 : 16);
        SetTextAttribute(UI_TEXT, UI_BG);
        Print(L"CaYaISOBoot\r\n");
    } else {
        SetTextAttribute(UI_ACCENT, UI_BG);
        Print(L"[CaYaDev] ");
        SetTextAttribute(UI_TEXT, UI_BG);
        Print(L"CaYaISOBoot\r\n");
    }
    SetTextAttribute(UI_MUTED, UI_BG);
    Print(L"Native Multi-ISO USB Boot System\r\n");
    Print(L"CaYa Native Boot Engine\r\n\r\n");
}

static const CHAR16 *TypeName(BOOT_FILE_TYPE Type) {
    if (Type == BootFileIso) {
        return L"ISO";
    }
    if (Type == BootFileImg) {
        return L"IMG";
    }
    return L"EFI";
}

#if 0
static const CHAR16 *EntryStatus(BOOT_FILE_TYPE Type) {
    if (Type == BootFileEfi) {
        return L"Doğrudan başlatılabilir";
    }
    if (Type == BootFileIso) {
        return L"ISO Handler gerekli";
    }
        return L"Raw Image Handler gerekli";
}
#endif

static BOOT_FILE_TYPE DetectType(const CHAR16 *Name, BOOLEAN *Supported) {
    const CHAR16 *Ext = 0;
    UINTN Index;
    for (Index = 0; Name[Index] != 0; Index++) {
        if (Name[Index] == L'.') {
            Ext = &Name[Index];
        }
    }
    if (Ext == 0) {
        *Supported = 0;
        return BootFileIso;
    }
    if (StriCmp(Ext, L".iso") == 0) {
        *Supported = 1;
        return BootFileIso;
    }
    if (StriCmp(Ext, L".img") == 0) {
        *Supported = 1;
        return BootFileImg;
    }
    if (StriCmp(Ext, L".efi") == 0) {
        *Supported = 1;
        return BootFileEfi;
    }
    *Supported = 0;
    return BootFileIso;
}

static void BuildPath(CHAR16 *Path, UINTN PathChars, const CHAR16 *Directory, const CHAR16 *Name) {
    CopyString(Path, PathChars, Directory);
    if (StrCmp(Directory, L"\\") != 0) {
        AppendString(Path, PathChars, L"\\");
    }
    AppendString(Path, PathChars, Name);
}

static EFI_STATUS AddEntry(BOOT_LIST *List, EFI_FILE_PROTOCOL *Root, const CHAR16 *Directory, EFI_FILE_INFO *Info, BOOT_FILE_TYPE Type) {
    BOOT_ENTRY *Entry;
    if (List->Count >= MAX_ENTRIES) {
        List->Truncated = 1;
        return EFI_BUFFER_TOO_SMALL;
    }
    Entry = &List->Entries[List->Count];
    Zero(Entry, sizeof(BOOT_ENTRY));
    Entry->Root = Root;
    CopyString(Entry->Name, MAX_NAME, Info->FileName);
    BuildPath(Entry->Path, MAX_PATH, Directory, Info->FileName);
    Entry->Size = Info->FileSize;
    Entry->Type = Type;
    List->Count++;
    return EFI_SUCCESS;
}

static EFI_STATUS OpenDirectory(EFI_FILE_PROTOCOL *Root, const CHAR16 *Path, EFI_FILE_PROTOCOL **Directory) {
    if (StrCmp(Path, L"\\") == 0) {
        Root->SetPosition(Root, 0);
        *Directory = Root;
        return EFI_SUCCESS;
    }
    return Root->Open(Root, Directory, (CHAR16 *)Path, EFI_FILE_MODE_READ, 0);
}

static EFI_STATUS ScanDirectory(BOOT_LIST *List, EFI_FILE_PROTOCOL *Root, const CHAR16 *DirectoryPath) {
    EFI_FILE_PROTOCOL *Directory;
    EFI_STATUS Status;
    UINT8 Buffer[2048];
    EFI_FILE_INFO *Info = (EFI_FILE_INFO *)Buffer;

    Status = OpenDirectory(Root, DirectoryPath, &Directory);
    if (EFI_ERROR(Status)) {
        return Status;
    }

    for (;;) {
        UINTN BufferSize = sizeof(Buffer);
        BOOLEAN Supported = 0;
        BOOT_FILE_TYPE Type;
        Zero(Buffer, sizeof(Buffer));
        Status = Directory->Read(Directory, &BufferSize, Buffer);
        if (EFI_ERROR(Status) || BufferSize == 0) {
            break;
        }
        if ((Info->Attribute & EFI_FILE_DIRECTORY) != 0) {
            continue;
        }
        Type = DetectType(Info->FileName, &Supported);
        if (!Supported) {
            continue;
        }
        Status = AddEntry(List, Root, DirectoryPath, Info, Type);
        if (Status == EFI_BUFFER_TOO_SMALL) {
            Status = EFI_SUCCESS;
            break;
        }
    }

    if (Directory != Root) {
        Directory->Close(Directory);
    }
    return EFI_ERROR(Status) ? Status : EFI_SUCCESS;
}

static void CloseListRoots(BOOT_LIST *List) {
    UINTN Index;
    for (Index = 0; Index < List->OpenedRootCount; Index++) {
        if (List->OpenedRoots[Index] != 0) {
            List->OpenedRoots[Index]->Close(List->OpenedRoots[Index]);
        }
    }
    List->OpenedRootCount = 0;
}

static EFI_STATUS ScanVolume(BOOT_LIST *List, EFI_FILE_PROTOCOL *Root) {
    ScanDirectory(List, Root, L"\\");
    ScanDirectory(List, Root, L"\\ISO");
    ScanDirectory(List, Root, L"\\CaYaISOBoot\\ISO");
    return EFI_SUCCESS;
}

/* ===================================================================
   Minimal read-only exFAT Block I/O driver
   Used when UEFI firmware does not expose the exFAT partition via
   EFI_SIMPLE_FILE_SYSTEM_PROTOCOL (no native exFAT support in firmware).
   We read directly via EFI_BLOCK_IO_PROTOCOL.
   =================================================================== */

static UINT64 ExfatClusterLba(EXFAT_VOL *V, UINT32 C) {
    return V->HeapLba + (UINT64)(C - 2u) * (UINT64)(V->BytesPerCluster / V->BytesPerSector);
}

static EFI_STATUS ExfatGetNextCluster(EXFAT_VOL *V, UINT32 Cur, UINT32 *Next) {
    UINT8      FatBuf[4096];
    UINT64     FatSectorLba = V->FatLba + (UINT64)Cur * 4u / V->BytesPerSector;
    UINT32     FatOff       = (UINT32)((UINT64)Cur * 4u % V->BytesPerSector);
    UINT32     BufBytes     = V->BytesPerSector <= 4096u ? V->BytesPerSector : 4096u;
    EFI_STATUS Status       = V->Bio->ReadBlocks(V->Bio, V->MediaId, FatSectorLba, BufBytes, FatBuf);
    if (EFI_ERROR(Status)) return Status;
    *Next = (UINT32)FatBuf[FatOff    ]         |
            ((UINT32)FatBuf[FatOff + 1] <<  8) |
            ((UINT32)FatBuf[FatOff + 2] << 16) |
            ((UINT32)FatBuf[FatOff + 3] << 24);
    return EFI_SUCCESS;
}

/* Read Size bytes from an exFAT file at byte offset Offset.
   NoFatChain=1: clusters are contiguous — O(1) seek, no FAT traversal. */
static EFI_STATUS ExfatReadFile(EXFAT_VOL *V, UINT32 FirstCluster, UINT8 NoFatChain,
                                 UINT64 Offset, UINTN Size, void *OutBuf)
{
    UINT8      *ClusterBuf;
    UINT8      *Out        = (UINT8 *)OutBuf;
    UINTN       Remaining  = Size;
    UINT64      ClusterIdx = Offset / V->BytesPerCluster;
    UINT32      ClusterOff = (UINT32)(Offset % V->BytesPerCluster);
    UINT32      Cluster    = FirstCluster;
    EFI_STATUS  Status;
    UINTN       i;

    if (Size == 0) return EFI_SUCCESS;

    Status = gBS->AllocatePool(EFI_LOADER_DATA, V->BytesPerCluster, (void **)&ClusterBuf);
    if (EFI_ERROR(Status)) return Status;

    for (i = 0; i < (UINTN)ClusterIdx; i++) {
        if (NoFatChain) {
            Cluster++;
        } else {
            UINT32 Next;
            Status = ExfatGetNextCluster(V, Cluster, &Next);
            if (EFI_ERROR(Status)) { gBS->FreePool(ClusterBuf); return Status; }
            if (Next >= EXFAT_FAT_EOC) { gBS->FreePool(ClusterBuf); return EFI_NOT_FOUND; }
            Cluster = Next;
        }
    }

    while (Remaining > 0) {
        UINTN Take;
        Status = V->Bio->ReadBlocks(V->Bio, V->MediaId,
                                    ExfatClusterLba(V, Cluster),
                                    V->BytesPerCluster, ClusterBuf);
        if (EFI_ERROR(Status)) { gBS->FreePool(ClusterBuf); return Status; }
        Take = (UINTN)(V->BytesPerCluster - ClusterOff);
        if (Take > Remaining) Take = Remaining;
        for (i = 0; i < Take; i++) Out[i] = ClusterBuf[ClusterOff + i];
        Out       += Take;
        Remaining -= Take;
        ClusterOff = 0;
        if (Remaining > 0) {
            UINT32 Next;
            if (NoFatChain) {
                Next = Cluster + 1;
            } else {
                Status = ExfatGetNextCluster(V, Cluster, &Next);
                if (EFI_ERROR(Status)) { gBS->FreePool(ClusterBuf); return Status; }
                if (Next >= EXFAT_FAT_EOC) { gBS->FreePool(ClusterBuf); return EFI_NOT_FOUND; }
            }
            Cluster = Next;
        }
    }

    gBS->FreePool(ClusterBuf);
    return EFI_SUCCESS;
}

/* Open an exFAT volume from a Block I/O handle.
   Returns TRUE if the "EXFAT   " VBR signature is found and fields are valid. */
static BOOLEAN ExfatOpenVol(EFI_BLOCK_IO_PROTOCOL *Bio, EXFAT_VOL *Vol) {
    UINT8  Vbr[4096];
    UINT32 ReadSize;
    UINT8  BPS, CPS;
    UINTN  i;
    static const UINT8 ExfatSig[8] = {'E','X','F','A','T',' ',' ',' '};

    if (!Bio->Media->MediaPresent) return 0;
    ReadSize = Bio->Media->BlockSize;
    if (ReadSize < 512u || ReadSize > 4096u) return 0;
    if (EFI_ERROR(Bio->ReadBlocks(Bio, Bio->Media->MediaId, 0, ReadSize, Vbr))) return 0;

    for (i = 0; i < 8; i++) if (Vbr[3 + i] != ExfatSig[i]) return 0;
    for (i = 11; i < 64; i++) if (Vbr[i] != 0) return 0; /* MustBeZero field */

    BPS = Vbr[0x6C]; /* BytesPerSectorShift */
    CPS = Vbr[0x6D]; /* SectorsPerClusterShift */
    if (BPS < 9u || BPS > 12u || (UINT32)CPS > 25u - BPS) return 0;

    Vol->Bio             = Bio;
    Vol->MediaId         = Bio->Media->MediaId;
    Vol->BytesPerSector  = 1u << BPS;
    Vol->BytesPerCluster = (1u << BPS) << CPS;
    Vol->FatLba  = (UINT64)((UINT32)Vbr[0x50]|((UINT32)Vbr[0x51]<<8)|((UINT32)Vbr[0x52]<<16)|((UINT32)Vbr[0x53]<<24));
    Vol->HeapLba = (UINT64)((UINT32)Vbr[0x58]|((UINT32)Vbr[0x59]<<8)|((UINT32)Vbr[0x5A]<<16)|((UINT32)Vbr[0x5B]<<24));
    Vol->RootCluster = (UINT32)Vbr[0x60]|((UINT32)Vbr[0x61]<<8)|((UINT32)Vbr[0x62]<<16)|((UINT32)Vbr[0x63]<<24);
    return 1;
}

/* Scan one exFAT root directory cluster chain. Add .iso/.img/.efi files to List. */
static void ExfatScanDirCluster(EXFAT_VOL *Vol, EXFAT_VOL *VolPtr, UINT32 DirCluster, BOOT_LIST *List) {
    UINT8 *ClusterBuf;
    UINT32 Cluster = DirCluster;

    if (EFI_ERROR(gBS->AllocatePool(EFI_LOADER_DATA, Vol->BytesPerCluster, (void **)&ClusterBuf))) return;

    while (Cluster >= 2u && Cluster < EXFAT_FAT_EOC) {
        UINT32 Offset;
        UINT32 NextC = EXFAT_FAT_EOC;

        if (EFI_ERROR(Vol->Bio->ReadBlocks(Vol->Bio, Vol->MediaId,
                ExfatClusterLba(Vol, Cluster), Vol->BytesPerCluster, ClusterBuf))) break;

        for (Offset = 0; Offset + 32u <= Vol->BytesPerCluster; ) {
            UINT8 *E = ClusterBuf + Offset;
            UINT8  SecCnt, IsDir, NameLen, NoFatChain;
            UINT16 Attr;
            UINT32 FileCluster;
            UINT64 FileSize;
            CHAR16 FileName[MAX_NAME];
            UINTN  FnPos, j, k;

            if (E[0] == 0x00u) goto ExfatDirDone;
            if ((E[0] & 0x80u) == 0) { Offset += 32u; continue; }
            if (E[0] != EXFAT_ENTRY_FILE) { Offset += 32u; continue; }

            SecCnt = E[1];
            Attr   = (UINT16)(E[4] | ((UINT16)E[5] << 8));
            IsDir  = (Attr & EXFAT_ATTR_DIRECTORY) ? 1u : 0u;

            /* Secondary entries must not cross cluster boundary for this scanner */
            if (SecCnt < 2u || Offset + 32u + (UINTN)SecCnt * 32u > Vol->BytesPerCluster) {
                Offset += (1u + (UINTN)SecCnt) * 32u;
                continue;
            }

            NameLen = 0; NoFatChain = 0; FileCluster = 0; FileSize = 0; FnPos = 0;

            for (j = 1; j <= SecCnt; j++) {
                UINT8 *SE = ClusterBuf + Offset + j * 32u;
                if (SE[0] == EXFAT_ENTRY_STREAM && j == 1) {
                    NoFatChain  = (SE[1] & 0x02u) ? 1u : 0u;
                    NameLen     = SE[3];
                    FileCluster = (UINT32)SE[20]|((UINT32)SE[21]<<8)|((UINT32)SE[22]<<16)|((UINT32)SE[23]<<24);
                    FileSize    = (UINT64)SE[24]|((UINT64)SE[25]<<8)|((UINT64)SE[26]<<16)|((UINT64)SE[27]<<24)|
                                  ((UINT64)SE[28]<<32)|((UINT64)SE[29]<<40)|((UINT64)SE[30]<<48)|((UINT64)SE[31]<<56);
                } else if (SE[0] == EXFAT_ENTRY_NAME) {
                    for (k = 0; k < 15u && FnPos < MAX_NAME - 1u; k++) {
                        CHAR16 Ch = (CHAR16)((UINT16)SE[2 + k*2u] | ((UINT16)SE[3 + k*2u] << 8));
                        if (Ch == 0) break;
                        FileName[FnPos++] = Ch;
                    }
                }
            }
            FileName[FnPos] = 0;

            if (!IsDir && NameLen > 0 && FileCluster >= 2u && List->Count < MAX_ENTRIES) {
                BOOLEAN Supported = 0;
                BOOT_FILE_TYPE FType = DetectType(FileName, &Supported);
                if (Supported) {
                    BOOT_ENTRY *Ent = &List->Entries[List->Count];
                    Zero(Ent, sizeof(BOOT_ENTRY));
                    Ent->ExVol        = VolPtr;
                    Ent->ExCluster    = FileCluster;
                    Ent->ExNoFatChain = NoFatChain;
                    Ent->Size         = FileSize;
                    Ent->Type         = FType;
                    CopyString(Ent->Name, MAX_NAME, FileName);
                    CopyString(Ent->Path, MAX_PATH, L"\\");
                    AppendString(Ent->Path, MAX_PATH, FileName);
                    List->Count++;
                }
            } else if (List->Count >= MAX_ENTRIES) {
                List->Truncated = 1;
            }
            Offset += (1u + (UINTN)SecCnt) * 32u;
        }
        ExfatGetNextCluster(Vol, Cluster, &NextC);
        Cluster = NextC;
    }
ExfatDirDone:
    gBS->FreePool(ClusterBuf);
}

/* Enumerate Block I/O logical partitions, detect exFAT volumes, add files to list. */
static void ScanExfatVolumes(BOOT_LIST *List) {
    UINTN       BufSize = 0;
    EFI_HANDLE *Handles = 0;
    EFI_STATUS  Status;
    UINTN       HandleCount, Index;

    Status = gBS->LocateHandle(ByProtocol, &BlockIoProtocolGuid, 0, &BufSize, 0);
    if (Status != EFI_BUFFER_TOO_SMALL || BufSize == 0) return;

    Status = gBS->AllocatePool(EFI_LOADER_DATA, BufSize, (void **)&Handles);
    if (EFI_ERROR(Status)) return;

    Status = gBS->LocateHandle(ByProtocol, &BlockIoProtocolGuid, 0, &BufSize, Handles);
    if (EFI_ERROR(Status)) { gBS->FreePool(Handles); return; }

    HandleCount = BufSize / sizeof(EFI_HANDLE);
    for (Index = 0; Index < HandleCount && List->ExfatVolCount < MAX_EXFAT_VOLS; Index++) {
        EFI_BLOCK_IO_PROTOCOL *Bio = 0;
        EXFAT_VOL *Vol;
        UINTN BeforeCount;

        Status = gBS->HandleProtocol(Handles[Index], &BlockIoProtocolGuid, (void **)&Bio);
        if (EFI_ERROR(Status) || !Bio->Media->MediaPresent) continue;
        if (!Bio->Media->LogicalPartition) continue; /* skip raw disk handles */

        Vol = &List->ExfatVols[List->ExfatVolCount];
        if (!ExfatOpenVol(Bio, Vol)) continue;

        BeforeCount = List->Count;
        ExfatScanDirCluster(Vol, Vol, Vol->RootCluster, List);
        if (List->Count > BeforeCount) List->ExfatVolCount++;
    }
    gBS->FreePool(Handles);
}

/* Remove exFAT entries that duplicate a SimpleFileSystem entry (same name+size).
   Prevents double-listing when firmware supports exFAT natively. */
static void RemoveExfatDuplicates(BOOT_LIST *List) {
    UINTN i, j;
    for (i = 0; i < List->Count; ) {
        BOOLEAN Removed = 0;
        if (List->Entries[i].ExVol == 0) { i++; continue; }
        for (j = 0; j < List->Count; j++) {
            if (i == j || List->Entries[j].ExVol != 0) continue;
            if (StrCmp(List->Entries[i].Name, List->Entries[j].Name) == 0 &&
                List->Entries[i].Size == List->Entries[j].Size) {
                UINTN k;
                for (k = i; k + 1 < List->Count; k++) List->Entries[k] = List->Entries[k + 1];
                List->Count--;
                Removed = 1;
                break;
            }
        }
        if (!Removed) i++;
    }
}

/* Loads a UEFI driver (e.g. exfat.efi) from the boot volume and connects it
   to all Block I/O handles so that new file systems become visible. */
static void TryLoadUefiDriver(EFI_FILE_PROTOCOL *Root, const CHAR16 *DriverPath) {
    EFI_FILE_PROTOCOL *File = 0;
    EFI_STATUS Status;
    UINT8 InfoBuf[512];
    UINTN InfoSize = sizeof(InfoBuf);
    EFI_FILE_INFO *FInfo;
    UINTN FileSize;
    void *Buffer = 0;
    EFI_HANDLE DriverHandle = 0;
    UINTN ExitDataSize = 0;
    UINTN BufSize = 0;
    EFI_HANDLE *Handles = 0;
    UINTN Cnt, Idx;

    Status = Root->Open(Root, &File, (CHAR16 *)DriverPath, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(Status)) return; /* driver not present — silent skip */

    Status = File->GetInfo(File, &FileInfoGuid, &InfoSize, InfoBuf);
    if (EFI_ERROR(Status)) { File->Close(File); return; }
    FInfo = (EFI_FILE_INFO *)InfoBuf;
    FileSize = (UINTN)FInfo->FileSize;
    if (FileSize == 0 || FileSize > 8 * 1024 * 1024) { File->Close(File); return; }

    Status = gBS->AllocatePool(EFI_LOADER_DATA, FileSize, &Buffer);
    if (EFI_ERROR(Status)) { File->Close(File); return; }

    Status = File->Read(File, &FileSize, Buffer);
    File->Close(File);
    if (EFI_ERROR(Status)) { gBS->FreePool(Buffer); return; }

    Status = gBS->LoadImage(0, gImageHandle, 0, Buffer, FileSize, &DriverHandle);
    gBS->FreePool(Buffer);
    if (EFI_ERROR(Status)) return;

    gBS->StartImage(DriverHandle, &ExitDataSize, 0);

    /* Re-connect all Block I/O handles so the new driver binds to exFAT partitions */
    Status = gBS->LocateHandle(ByProtocol, &BlockIoProtocolGuid, 0, &BufSize, 0);
    if (Status == EFI_BUFFER_TOO_SMALL && BufSize > 0) {
        Status = gBS->AllocatePool(EFI_LOADER_DATA, BufSize, (void **)&Handles);
        if (!EFI_ERROR(Status)) {
            Status = gBS->LocateHandle(ByProtocol, &BlockIoProtocolGuid, 0, &BufSize, Handles);
            if (!EFI_ERROR(Status)) {
                Cnt = BufSize / sizeof(EFI_HANDLE);
                for (Idx = 0; Idx < Cnt; Idx++) {
                    gBS->ConnectController(Handles[Idx], 0, 0, 1);
                }
            }
            gBS->FreePool(Handles);
        }
    }
}

static EFI_STATUS ScanFiles(BOOT_LIST *List) {
    EFI_STATUS Status;
    UINTN BufferSize = 0;
    EFI_HANDLE *Handles = 0;
    UINTN HandleCount;
    UINTN Index;

    CloseListRoots(List);
    Zero(List, sizeof(BOOT_LIST));

    Status = gBS->LocateHandle(ByProtocol, &SimpleFileSystemProtocolGuid, 0, &BufferSize, 0);
    if (Status != EFI_BUFFER_TOO_SMALL || BufferSize == 0) {
        ScanVolume(List, gRoot);
        goto ScanDone;
    }

    Status = gBS->AllocatePool(EFI_LOADER_DATA, BufferSize, (void **)&Handles);
    if (EFI_ERROR(Status)) {
        ScanVolume(List, gRoot);
        goto ScanDone;
    }

    Status = gBS->LocateHandle(ByProtocol, &SimpleFileSystemProtocolGuid, 0, &BufferSize, Handles);
    if (EFI_ERROR(Status)) {
        gBS->FreePool(Handles);
        Handles = 0;
        ScanVolume(List, gRoot);
        goto ScanDone;
    }

    HandleCount = BufferSize / sizeof(EFI_HANDLE);
    for (Index = 0; Index < HandleCount && List->OpenedRootCount < MAX_VOLUME_ROOTS; Index++) {
        EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFs;
        EFI_FILE_PROTOCOL *Root;
        UINTN BeforeCount;

        Status = gBS->HandleProtocol(Handles[Index], &SimpleFileSystemProtocolGuid, (void **)&SimpleFs);
        if (EFI_ERROR(Status)) continue;

        Status = SimpleFs->OpenVolume(SimpleFs, &Root);
        if (EFI_ERROR(Status)) continue;

        BeforeCount = List->Count;
        ScanVolume(List, Root);
        if (List->Count > BeforeCount) {
            List->OpenedRoots[List->OpenedRootCount++] = Root;
        } else {
            Root->Close(Root);
        }
    }

    gBS->FreePool(Handles);

ScanDone:
    ScanExfatVolumes(List);
    RemoveExfatDuplicates(List);
    return EFI_SUCCESS;
}

#if 0
static void DrawMenu(BOOT_LIST *List, UINTN Selected) {
    UINTN Index;
    Clear();
    Header();
    Print(L"Bulunan boot dosyaları:\r\n\r\n");
    if (List->Count == 0) {
        Print(L"USB üzerinde .iso, .img veya .efi dosyası bulunamadı.\r\n\r\n");
    }
    for (Index = 0; Index < List->Count; Index++) {
        BOOT_ENTRY *Entry = &List->Entries[Index];
        Print(Index == Selected ? L"> " : L"  ");
        Print(L"[");
        PrintUInt(Index + 1);
        Print(L"] ");
        Print(Entry->Name);
        Print(L"\r\n    Tür: ");
        Print((CHAR16 *)TypeName(Entry->Type));
        Print(L"\r\n    Konum: ");
        Print(Entry->Path);
        Print(L"\r\n    Durum: ");
        Print((CHAR16 *)EntryStatus(Entry->Type));
        Print(L"\r\n\r\n");
    }
    if (List->Truncated) {
        Print(L"Not: Liste maksimum kapasiteye ulaştığı için kırpıldı.\r\n\r\n");
    }
    Print(L"Tuşlar: Yukarı/Aşağı seçim, Enter başlat, R yenile, I bilgi, Esc çıkış\r\n");
}

static void ShowEntryInfo(BOOT_ENTRY *Entry) {
    Clear();
    Header();
    Print(L"Dosya bilgisi\r\n\r\n");
    Print(L"Ad: ");
    Print(Entry->Name);
    Print(L"\r\nKonum: ");
    Print(Entry->Path);
    Print(L"\r\nTür: ");
    Print((CHAR16 *)TypeName(Entry->Type));
    Print(L"\r\nBoyut: ");
    PrintUInt(Entry->Size);
    Print(L" bayt\r\nDurum: ");
    Print((CHAR16 *)EntryStatus(Entry->Type));
    Print(L"\r\n\r\nDevam etmek için bir tuşa basın.\r\n");
    WaitKey();
}

static EFI_STATUS ReadFileToMemory(BOOT_ENTRY *Entry, void **Buffer, UINTN *Size) {
    EFI_STATUS Status;
    EFI_FILE_PROTOCOL *File;
    UINTN ReadSize;

    if (Entry->Size == 0 || Entry->Size > MAX_EFI_LOAD_SIZE) {
        return EFI_UNSUPPORTED;
    }

    Status = Entry->Root->Open(Entry->Root, &File, Entry->Path, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(Status)) {
        return Status;
    }

    Status = gBS->AllocatePool(EFI_LOADER_DATA, (UINTN)Entry->Size, Buffer);
    if (EFI_ERROR(Status)) {
        File->Close(File);
        return Status;
    }

    ReadSize = (UINTN)Entry->Size;
    Status = File->Read(File, &ReadSize, *Buffer);
    File->Close(File);

    if (EFI_ERROR(Status) || ReadSize != (UINTN)Entry->Size) {
        gBS->FreePool(*Buffer);
        *Buffer = 0;
        return EFI_LOAD_ERROR;
    }

    *Size = ReadSize;
    return EFI_SUCCESS;
}

static EFI_STATUS StartEfi(BOOT_ENTRY *Entry) {
    EFI_STATUS Status;
    EFI_HANDLE Child = 0;
    void *Buffer = 0;
    UINTN Size = 0;

    Clear();
    Header();
    Print(L"EFI başlatılıyor: ");
    Print(Entry->Path);
    Print(L"\r\n");

    Status = ReadFileToMemory(Entry, &Buffer, &Size);
    if (EFI_ERROR(Status)) {
        Print(L"EFI dosyası okunamadı veya çok büyük.\r\n");
        return Status;
    }

    Status = gBS->LoadImage(0, gImageHandle, 0, Buffer, Size, &Child);
    if (EFI_ERROR(Status)) {
        Print(L"LoadImage başarısız oldu.\r\n");
        gBS->FreePool(Buffer);
        return Status;
    }

    Status = gBS->StartImage(Child, 0, 0);
    gBS->FreePool(Buffer);
    return Status;
}

static void HandleIsoOrImg(BOOT_ENTRY *Entry) {
    Clear();
    Header();
    Print(L"Seçilen dosya: ");
    Print(Entry->Name);
    Print(L"\r\nKonum: ");
    Print(Entry->Path);
    Print(L"\r\nTür: ");
    Print((CHAR16 *)TypeName(Entry->Type));
    Print(L"\r\n\r\n");
    if (Entry->Type == BootFileIso) {
        Print(L"ISO algılandı, fakat MVP ISO boot işlemini henüz uygulamaz.\r\n");
        Print(L"Gerekli sonraki parçalar: ISO9660 okuyucu, El Torito analizi,\r\n");
        Print(L"UEFI boot image tespiti ve işletim sistemi profilleri.\r\n");
    } else {
        Print(L"Raw IMG algılandı, fakat MVP raw image boot işlemini henüz uygulamaz.\r\n");
    }
    Print(L"\r\nSahte başarı üretilmedi. Devam etmek için bir tuşa basın.\r\n");
    WaitKey();
}
#endif

static const CHAR16 *EntryStatusAscii(BOOT_FILE_TYPE Type) {
    if (Type == BootFileEfi) {
        return L"Direct EFI boot";
    }
    if (Type == BootFileIso) {
        return L"UEFI ISO boot (El Torito / EFI path)";
    }
    return L"Raw image handler not yet implemented";
}

static UINTN MenuStartIndex(UINTN Count, UINTN Selected) {
    if (Count <= MENU_VISIBLE_ROWS || Selected < MENU_VISIBLE_ROWS) {
        return 0;
    }
    if (Selected + 1 >= Count) {
        return Count - MENU_VISIBLE_ROWS;
    }
    return Selected - MENU_VISIBLE_ROWS + 1;
}

static UINTN MenuVisibleCount(UINTN Count, UINTN Start) {
    UINTN Remaining = Count > Start ? Count - Start : 0;
    return Remaining > MENU_VISIBLE_ROWS ? MENU_VISIBLE_ROWS : Remaining;
}

/* CaYa Red full-width separator (80 chars) drawn with '=' in red. */
static const CHAR16 *kRedRule  = L"================================================================================";
/* Soft separator (80 chars) drawn with '-' in dark gray. */
static const CHAR16 *kSoftRule = L"--------------------------------------------------------------------------------";

static void PrintRepeated(CHAR16 Ch, UINTN Count) {
    CHAR16 Buffer[96];
    UINTN Index;

    while (Count > 0) {
        UINTN Chunk = Count > 95 ? 95 : Count;
        for (Index = 0; Index < Chunk; Index++) {
            Buffer[Index] = Ch;
        }
        Buffer[Chunk] = 0;
        Print(Buffer);
        Count -= Chunk;
    }
}

static void DrawBox(UINTN Column, UINTN Row, UINTN Width, UINTN Height, const CHAR16 *Title) {
    UINTN Index;
    UINTN TitleLen = Title != 0 ? StrLen(Title) : 0;
    UINTN Fill;

    if (Width < 4 || Height < 3) {
        return;
    }

    SetTextAttribute(UI_DIM, UI_BG);
    SetCursor(Column, Row);
    Print(L"+");
    if (TitleLen > 0 && TitleLen + 4 < Width) {
        Print(L" ");
        SetTextAttribute(UI_TEXT, UI_BG);
        Print(Title);
        SetTextAttribute(UI_DIM, UI_BG);
        Print(L" ");
        Fill = Width - TitleLen - 4;
    } else {
        Fill = Width - 2;
    }
    PrintRepeated(L'-', Fill);
    Print(L"+");

    for (Index = 1; Index + 1 < Height; Index++) {
        SetTextAttribute(UI_DIM, UI_BG);
        PrintAt(Column, Row + Index, L"|");
        PrintAt(Column + Width - 1, Row + Index, L"|");
    }

    SetCursor(Column, Row + Height - 1);
    Print(L"+");
    PrintRepeated(L'-', Width - 2);
    Print(L"+");
}

static void DrawTopText(const CHAR16 *ModeText) {
    UINTN TitleColumn = 12;

    if (gGop != 0) {
        UINTN Scale = LogoScale();
        TitleColumn = Scale >= 2 ? 33 : 25;
        DrawCaYaDevLogo(16, Scale >= 2 ? 14 : 16);
    } else {
        /* Text-only fallback for firmware without GOP. */
        SetTextAttribute(UI_ACCENT, UI_BG);
        PrintAt(2, 1, L"[CaYaDev]");
    }

    SetTextAttribute(UI_TEXT, UI_BG);
    PrintAt(TitleColumn, 1, L"CaYaISOBoot");
    SetTextAttribute(UI_MUTED, UI_BG);
    PrintAt(gGop != 0 ? TitleColumn : 2, 2, L"Native Multi-ISO USB Boot System");
    SetTextAttribute(UI_MUTED, UI_BG);
    PrintAt(gGop != 0 ? TitleColumn : 2, 3, ModeText);
    SetTextAttribute(UI_TEXT, UI_SELECTED_BG);
    PrintAt(68, 1, L" Menu ");
    SetTextAttribute(UI_ACCENT, UI_BG);
    PrintAt(0, 4, kRedRule);
    SetTextAttribute(UI_MUTED, UI_BG);
}

static void DrawBottomBar(const CHAR16 *Keys) {
    /* Soft separator on row 21, footer keys on row 22 — anchored to the
       bottom of a standard 80x25 EFI console. */
    if (gGop == 0) {
        SetTextAttribute(UI_DIM, UI_BG);
        PrintAt(0, 21, kSoftRule);
    }
    SetTextAttribute(UI_TEXT, UI_BG);
    PrintAt(2, 22, Keys);
    SetTextAttribute(UI_MUTED, UI_BG);
    PrintAt(68, 23, L"cayadev.com");
}

static void DrawStartupScreen(void) {
    Clear();
    DrawGraphicChrome(0, 0);
    HideCursor();
    DrawTopText(gGop != 0 ? L"Graphical UI (GOP)" : L"CaYa Native Boot Engine - Text UI");
    SetTextAttribute(UI_MUTED, UI_BG);
    PrintAt(2, 6, L"Opening USB file systems...");
}

/* Pad text-mode columns by overwriting the row range with spaces using the
   currently active background attribute. Used to render the selection bar as
   a flat-colored band that spans the full panel width. */
static void PadSpaces(UINTN Count) {
    CHAR16 Buffer[64];
    UINTN Index;
    if (Count == 0) {
        return;
    }
    if (Count > 63) {
        Count = 63;
    }
    for (Index = 0; Index < Count; Index++) {
        Buffer[Index] = L' ';
    }
    Buffer[Count] = 0;
    Print(Buffer);
}

static void DrawMenuAscii(BOOT_LIST *List, UINTN Selected) {
    UINTN Start = MenuStartIndex(List->Count, Selected);
    UINTN Visible = MenuVisibleCount(List->Count, Start);
    UINTN Index;
    const UINTN LeftCol = 2;
    const UINTN LeftW = 42;
    const UINTN RightCol = 46;
    const UINTN RightW = 32;
    const UINTN ListTop = 8;
    const UINTN PanelBg = UI_BG;

    Clear();
    DrawGraphicChrome(Visible, List->Count > 0 ? Selected - Start : 0);
    HideCursor();
    DrawTopText(gGop != 0 ? L"Graphical UI (GOP)" : L"CaYa Native Boot Engine - Text UI");

    SetTextAttribute(UI_TEXT, PanelBg);
    PrintAt(LeftCol, 6, L"Boot files");
    PrintAt(RightCol, 6, L"Selected entry");
    SetTextAttribute(UI_DIM, PanelBg);
    PrintAt(LeftCol, 7, L"ISO / IMG / EFI");
    PrintAt(RightCol, 7, L"Runtime details");

    if (List->Count == 0) {
        SetTextAttribute(EFI_YELLOW, PanelBg);
        PrintAt(LeftCol, 9, L"No bootable files found.");
        SetTextAttribute(UI_MUTED, PanelBg);
        PrintAt(LeftCol, 11, L"Copy files to CAYAISO root");
        PrintAt(LeftCol, 12, L"or /ISO, then press R.");
    }

    for (Index = Start; Index < Start + Visible; Index++) {
        BOOT_ENTRY *Entry = &List->Entries[Index];
        UINTN Row = ListTop + (Index - Start);
        BOOLEAN IsSelected = (Index == Selected);
        UINTN PrintedChars;
        UINTN NameLimit;

        SetTextAttribute(IsSelected ? UI_TEXT        : UI_MUTED,
                         IsSelected ? UI_SELECTED_BG : PanelBg);
        SetCursor(LeftCol, Row);
        Print(IsSelected ? L"> [" : L"  [");
        PrintUInt(Index + 1);
        Print(L"] ");

        /* Reserve room for prefix "> [NN] " (~7 chars) and pad the rest of
           the panel width with spaces so the selection background spans the
           whole row. */
        PrintedChars = 3 /* "> [" */ + 1 /* digit (rough) */ + 2 /* "] " */;
        if (Index + 1 >= 10)  PrintedChars++;
        if (Index + 1 >= 100) PrintedChars++;
        NameLimit = LeftW > PrintedChars ? LeftW - PrintedChars : 0;
        PrintLimited(Entry->Name, NameLimit);

        if (IsSelected) {
            /* Trail with spaces to extend the red bar to the panel edge. */
            UINTN NameLen = 0;
            while (NameLen < NameLimit && Entry->Name[NameLen] != 0) NameLen++;
            if (NameLen + PrintedChars < LeftW) {
                PadSpaces(LeftW - PrintedChars - NameLen);
            }
        }
    }

    /* Right panel: details for the currently selected entry. */
    if (List->Count > 0) {
        BOOT_ENTRY *Entry = &List->Entries[Selected];
        SetTextAttribute(UI_TEXT, PanelBg);
        PrintAt(RightCol,  9, L"Name");
        SetTextAttribute(UI_TEXT, PanelBg);
        PrintAt(RightCol + 7, 9, L": ");
        PrintLimited(Entry->Name, RightW - 7);
        SetTextAttribute(UI_TEXT, PanelBg);
        PrintAt(RightCol, 11, L"Type");
        SetTextAttribute(UI_MUTED, PanelBg);
        PrintAt(RightCol + 7, 11, L": ");
        Print((CHAR16 *)TypeName(Entry->Type));
        SetTextAttribute(UI_TEXT, PanelBg);
        PrintAt(RightCol, 12, L"Size");
        SetTextAttribute(UI_MUTED, PanelBg);
        PrintAt(RightCol + 7, 12, L": ");
        PrintUInt(Entry->Size);
        Print(L" bytes");
        SetTextAttribute(UI_TEXT, PanelBg);
        PrintAt(RightCol, 13, L"State");
        SetTextAttribute(UI_MUTED, PanelBg);
        PrintAt(RightCol + 7, 13, L": ");
        PrintLimited((CHAR16 *)EntryStatusAscii(Entry->Type), RightW - 7);
        SetTextAttribute(UI_TEXT, PanelBg);
        PrintAt(RightCol, 15, L"Path");
        SetTextAttribute(UI_MUTED, PanelBg);
        PrintAt(RightCol + 7, 15, L": ");
        PrintLimited(Entry->Path, RightW - 7);
        SetTextAttribute(UI_ACCENT, PanelBg);
        PrintAt(RightCol, 17, L"ENTER launches selected file");
    }

    /* Status line below both panels. */
    if (List->Truncated) {
        SetTextAttribute(EFI_YELLOW, UI_BG);
        PrintAt(2, 20, L"Warning: file list reached the maximum entry limit and was truncated.");
    } else if (List->Count > MENU_VISIBLE_ROWS) {
        SetTextAttribute(UI_MUTED, UI_BG);
        SetCursor(2, 20);
        Print(L"Showing ");
        PrintUInt(Start + 1);
        Print(L"-");
        PrintUInt(Start + Visible);
        Print(L" of ");
        PrintUInt(List->Count);
        Print(L" | Data: CAYAISO root or /ISO | System: CAYABOOT");
    } else if (List->Count > 0) {
        SetTextAttribute(UI_MUTED, UI_BG);
        PrintAt(2, 20, L"Data: CAYAISO root or /ISO | System: hidden CAYABOOT");
    }

    DrawBottomBar(L"Enter Boot     Up/Down Select     R Rescan     I Info     Esc Exit");
}

static void ShowEntryInfoAscii(BOOT_ENTRY *Entry) {
    Clear();
    DrawGraphicChrome(0, 0);
    HideCursor();
    DrawTopText(L"File information");
    DrawBox(1, 6, 78, 12, L"Selected entry");
    SetTextAttribute(EFI_LIGHTGRAY, EFI_BLACK);
    PrintAt(3,  8, L"Name : "); PrintLimited(Entry->Name, 68);
    PrintAt(3,  9, L"Path : "); PrintLimited(Entry->Path, 68);
    PrintAt(3, 11, L"Type : "); Print((CHAR16 *)TypeName(Entry->Type));
    PrintAt(3, 12, L"Size : "); PrintUInt(Entry->Size); Print(L" bytes");
    PrintAt(3, 13, L"State: "); PrintLimited((CHAR16 *)EntryStatusAscii(Entry->Type), 68);
    DrawBottomBar(L"Press any key to return.");
    WaitKey();
}

static EFI_STATUS ReadFileToMemoryAscii(BOOT_ENTRY *Entry, void **Buffer, UINTN *Size) {
    EFI_STATUS Status;
    EFI_FILE_PROTOCOL *File;
    UINTN ReadSize;

    if (Entry->Size == 0 || Entry->Size > MAX_EFI_LOAD_SIZE) {
        return EFI_UNSUPPORTED;
    }

    /* exFAT Block I/O path */
    if (Entry->ExVol != 0) {
        ReadSize = (UINTN)Entry->Size;
        Status = gBS->AllocatePool(EFI_LOADER_DATA, ReadSize, Buffer);
        if (EFI_ERROR(Status)) return Status;
        Status = ExfatReadFile(Entry->ExVol, Entry->ExCluster, Entry->ExNoFatChain, 0, ReadSize, *Buffer);
        if (EFI_ERROR(Status)) { gBS->FreePool(*Buffer); *Buffer = 0; return Status; }
        *Size = ReadSize;
        return EFI_SUCCESS;
    }

    Status = Entry->Root->Open(Entry->Root, &File, Entry->Path, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(Status)) {
        return Status;
    }

    Status = gBS->AllocatePool(EFI_LOADER_DATA, (UINTN)Entry->Size, Buffer);
    if (EFI_ERROR(Status)) {
        File->Close(File);
        return Status;
    }

    ReadSize = (UINTN)Entry->Size;
    Status = File->Read(File, &ReadSize, *Buffer);
    File->Close(File);

    if (EFI_ERROR(Status) || ReadSize != (UINTN)Entry->Size) {
        gBS->FreePool(*Buffer);
        *Buffer = 0;
        return EFI_LOAD_ERROR;
    }

    *Size = ReadSize;
    return EFI_SUCCESS;
}

static EFI_STATUS StartEfiAscii(BOOT_ENTRY *Entry) {
    EFI_STATUS Status;
    EFI_HANDLE Child = 0;
    void *Buffer = 0;
    UINTN Size = 0;

    Clear();
    DrawGraphicChrome(0, 0);
    HideCursor();
    DrawTopText(L"EFI chainload");
    SetTextAttribute(EFI_LIGHTGRAY, EFI_BLACK);
    PrintAt(2, 6, L"Starting EFI file: ");
    PrintLimited(Entry->Path, 60);

    Status = ReadFileToMemoryAscii(Entry, &Buffer, &Size);
    if (EFI_ERROR(Status)) {
        SetTextAttribute(EFI_LIGHTRED, EFI_BLACK);
        PrintAt(2, 8, L"EFI file could not be read, or it is larger than the MVP load limit.");
        return Status;
    }

    Status = gBS->LoadImage(0, gImageHandle, 0, Buffer, Size, &Child);
    if (EFI_ERROR(Status)) {
        SetTextAttribute(EFI_LIGHTRED, EFI_BLACK);
        PrintAt(2, 8, L"LoadImage failed. The file may not be a valid UEFI application.");
        gBS->FreePool(Buffer);
        return Status;
    }

    Status = gBS->StartImage(Child, 0, 0);
    gBS->FreePool(Buffer);
    return Status;
}

/* ===================================================================
   ISO 9660 + El Torito UEFI boot implementation

   ISO files live on a FAT32 partition as regular files.
   All sector reads use EFI_FILE_PROTOCOL SetPosition+Read.
   Byte offset in file = LBA * 2048.
   =================================================================== */

#define ISO_SECTOR_SIZE   2048u
#define ISO_PVD_LBA       16u
#define ELTORITO_BOOTABLE 0x88u
#define ELTORITO_UEFI     0xEFu

/* Read N consecutive 2048-byte ISO sectors from an open ISO file handle.
   LBA 0 = byte 0, LBA 16 = byte 32768, etc. */
/* ISO reader abstraction: works with FAT EFI_FILE_PROTOCOL or exFAT Block I/O. */
typedef struct {
    BOOLEAN            IsExfat;
    EFI_FILE_PROTOCOL *File;        /* FAT path */
    EXFAT_VOL         *ExVol;       /* exFAT path */
    UINT32             ExCluster;
    UINT8              ExNoFatChain;
} ISO_READER;

static EFI_STATUS IsoRead(ISO_READER *R, UINT32 Lba, UINT32 Count, void *Buf) {
    if (R->IsExfat) {
        return ExfatReadFile(R->ExVol, R->ExCluster, R->ExNoFatChain,
                             (UINT64)Lba * ISO_SECTOR_SIZE, (UINTN)Count * ISO_SECTOR_SIZE, Buf);
    }
    {
        UINTN ExpectedSize = (UINTN)Count * ISO_SECTOR_SIZE;
        UINTN ReadSize = ExpectedSize;
        EFI_STATUS St = R->File->SetPosition(R->File, (UINT64)Lba * ISO_SECTOR_SIZE);
        if (EFI_ERROR(St)) return St;
        St = R->File->Read(R->File, &ReadSize, Buf);
        if (EFI_ERROR(St)) return St;
        return ReadSize == ExpectedSize ? EFI_SUCCESS : EFI_LOAD_ERROR;
    }
}

static EFI_STATUS IsoReadBytes(ISO_READER *R, UINT64 Offset, UINTN Size, void *Buf) {
    if (Size == 0) {
        return EFI_SUCCESS;
    }
    if (R->IsExfat) {
        return ExfatReadFile(R->ExVol, R->ExCluster, R->ExNoFatChain, Offset, Size, Buf);
    }
    {
        UINTN ReadSize = Size;
        EFI_STATUS St = R->File->SetPosition(R->File, Offset);
        if (EFI_ERROR(St)) return St;
        St = R->File->Read(R->File, &ReadSize, Buf);
        if (EFI_ERROR(St)) return St;
        return ReadSize == Size ? EFI_SUCCESS : EFI_LOAD_ERROR;
    }
}

static EFI_STATUS IsoReadFromFile(EFI_FILE_PROTOCOL *File, UINT32 Lba, UINT32 Count, void *Buf) {
    EFI_STATUS Status;
    UINTN ExpectedSize = (UINTN)Count * ISO_SECTOR_SIZE;
    UINTN ReadSize = ExpectedSize;
    Status = File->SetPosition(File, (UINT64)Lba * (UINT64)ISO_SECTOR_SIZE);
    if (EFI_ERROR(Status)) return Status;
    Status = File->Read(File, &ReadSize, Buf);
    if (EFI_ERROR(Status)) return Status;
    return ReadSize == ExpectedSize ? EFI_SUCCESS : EFI_LOAD_ERROR;
}

static UINT16 PeRead16(UINT8 *Image, UINTN ImageSize, UINTN Offset) {
    if (Offset + 1u >= ImageSize) {
        return 0;
    }
    return (UINT16)(Image[Offset] | ((UINT16)Image[Offset + 1u] << 8u));
}

static UINT32 PeRead32(UINT8 *Image, UINTN ImageSize, UINTN Offset) {
    if (Offset + 3u >= ImageSize) {
        return 0;
    }
    return (UINT32)Image[Offset] |
           ((UINT32)Image[Offset + 1u] << 8u) |
           ((UINT32)Image[Offset + 2u] << 16u) |
           ((UINT32)Image[Offset + 3u] << 24u);
}

static EFI_STATUS ValidateX64UefiImage(UINT8 *Image, UINTN ImageSize, const CHAR16 **Reason) {
    UINT32 PeOffset;
    UINT16 Machine;
    UINT16 OptionalSize;
    UINT16 OptionalMagic;
    UINT16 Subsystem;
    UINTN OptionalOffset;

    if (Reason != 0) {
        *Reason = L"Invalid UEFI image.";
    }

    if (ImageSize < 0x100u || Image[0] != 'M' || Image[1] != 'Z') {
        if (Reason != 0) {
            *Reason = L"Extracted image is not a PE/COFF executable.";
        }
        return EFI_UNSUPPORTED;
    }

    PeOffset = PeRead32(Image, ImageSize, 0x3Cu);
    if (PeOffset < 0x40u || (UINTN)PeOffset + 0x18u >= ImageSize ||
        Image[PeOffset] != 'P' || Image[PeOffset + 1u] != 'E' ||
        Image[PeOffset + 2u] != 0 || Image[PeOffset + 3u] != 0) {
        if (Reason != 0) {
            *Reason = L"PE header is missing or corrupt.";
        }
        return EFI_UNSUPPORTED;
    }

    Machine = PeRead16(Image, ImageSize, (UINTN)PeOffset + 4u);
    if (Machine != 0x8664u) {
        if (Reason != 0) {
            if (Machine == 0x014Cu) {
                *Reason = L"Found IA32 EFI image; x64 UEFI cannot start it.";
            } else if (Machine == 0xAA64u) {
                *Reason = L"Found ARM64 EFI image; x64 UEFI cannot start it.";
            } else {
                *Reason = L"EFI image is not built for x64 UEFI.";
            }
        }
        return EFI_UNSUPPORTED;
    }

    OptionalSize = PeRead16(Image, ImageSize, (UINTN)PeOffset + 20u);
    OptionalOffset = (UINTN)PeOffset + 24u;
    if (OptionalSize < 70u || OptionalOffset + OptionalSize > ImageSize) {
        if (Reason != 0) {
            *Reason = L"PE optional header is missing or truncated.";
        }
        return EFI_UNSUPPORTED;
    }

    OptionalMagic = PeRead16(Image, ImageSize, OptionalOffset);
    if (OptionalMagic != 0x10Bu && OptionalMagic != 0x20Bu) {
        if (Reason != 0) {
            *Reason = L"PE optional header magic is unsupported.";
        }
        return EFI_UNSUPPORTED;
    }

    Subsystem = PeRead16(Image, ImageSize, OptionalOffset + 68u);
    if (Subsystem != 10u && Subsystem != 11u && Subsystem != 12u) {
        if (Reason != 0) {
            *Reason = L"PE file is not a UEFI application/driver.";
        }
        return EFI_UNSUPPORTED;
    }

    if (Reason != 0) {
        *Reason = L"OK";
    }
    return EFI_SUCCESS;
}

/* Load an EFI image from a raw memory buffer and start it.
   When ShowError is FALSE the LoadImage failure message is suppressed —
   used by callers that have a fallback path and don't want a misleading
   message overwriting later status text. */
static EFI_STATUS LoadAndStartFromBufferEx(void *ImageData, UINTN ImageSize, BOOLEAN ShowError) {
    EFI_STATUS Status;
    EFI_HANDLE Child = 0;
    const CHAR16 *ValidationError = 0;

    Status = ValidateX64UefiImage((UINT8 *)ImageData, ImageSize, &ValidationError);
    if (EFI_ERROR(Status)) {
        if (ShowError) {
            SetTextAttribute(EFI_LIGHTRED, EFI_BLACK);
            PrintAt(3, 18, ValidationError);
        }
        return Status;
    }

    Status = gBS->LoadImage(0, gImageHandle, 0, ImageData, ImageSize, &Child);
    if (EFI_ERROR(Status)) {
        if (ShowError) {
            SetTextAttribute(EFI_LIGHTRED, EFI_BLACK);
            PrintAt(3, 18, L"LoadImage failed - image may not be a valid UEFI binary.");
        }
        return Status;
    }
    Status = gBS->StartImage(Child, 0, 0);
    return Status;
}

static EFI_STATUS LoadAndStartFromBuffer(void *ImageData, UINTN ImageSize) {
    return LoadAndStartFromBufferEx(ImageData, ImageSize, 1);
}

#define EFI_NATIVE_INTERFACE 0u
#define EFI_BLOCK_IO_PROTOCOL_REVISION 0x00010000ULL
#define EFI_DISK_IO_PROTOCOL_REVISION  0x00010000ULL
#define MEDIA_DEVICE_PATH 0x04u
#define MEDIA_CDROM_DP 0x02u
#define MEDIA_VENDOR_DP 0x03u
#define MEDIA_FILEPATH_DP 0x04u
#define END_DEVICE_PATH_TYPE 0x7Fu
#define END_ENTIRE_DEVICE_PATH_SUBTYPE 0xFFu
#define CAYA_OFFSET_OF(TYPE, Field) ((UINTN)&(((TYPE *)0)->Field))

#pragma pack(push, 1)
typedef struct {
    EFI_DEVICE_PATH_PROTOCOL Header;
    EFI_GUID Guid;
} CAYA_VENDOR_DEVICE_PATH;

typedef struct {
    EFI_DEVICE_PATH_PROTOCOL Header;
    UINT32 BootEntry;
    UINT64 PartitionStart;
    UINT64 PartitionSize;
} CAYA_CDROM_DEVICE_PATH;

typedef struct {
    CAYA_VENDOR_DEVICE_PATH Vendor;
    CAYA_CDROM_DEVICE_PATH Cdrom;
    EFI_DEVICE_PATH_PROTOCOL End;
} CAYA_VENDOR_CDROM_END_DEVICE_PATH;

typedef struct {
    CAYA_VENDOR_DEVICE_PATH Vendor;
    EFI_DEVICE_PATH_PROTOCOL End;
} CAYA_VENDOR_END_DEVICE_PATH;
#pragma pack(pop)

typedef struct {
    EFI_BLOCK_IO_PROTOCOL BlockIo;
    EFI_DISK_IO_PROTOCOL DiskIo;
    EFI_BLOCK_IO_MEDIA Media;
    CAYA_VENDOR_CDROM_END_DEVICE_PATH DevicePath;
    UINT8 *Image;
    UINTN ImageSize;
} MEMORY_BLOCK_CONTEXT;

typedef struct {
    EFI_BLOCK_IO_PROTOCOL BlockIo;
    EFI_DISK_IO_PROTOCOL DiskIo;
    EFI_BLOCK_IO_MEDIA Media;
    /* Whole-CD virtual device must use Vendor+End; CDROM node represents a
       boot partition within a CD and confuses firmware ISO9660 drivers when
       used at the whole-disk level (Windows installer can't see boot.wim). */
    CAYA_VENDOR_END_DEVICE_PATH DevicePath;
    ISO_READER Reader;
    UINT64 ImageSize;
    EFI_HANDLE Handle;
} ISO_CD_CONTEXT;

static void SetDevicePathLength(EFI_DEVICE_PATH_PROTOCOL *Node, UINT16 Length) {
    Node->Length[0] = (UINT8)(Length & 0xFFu);
    Node->Length[1] = (UINT8)((Length >> 8u) & 0xFFu);
}

static UINTN RoundUpU(UINTN Value, UINTN Alignment) {
    if (Alignment == 0) return Value;
    return (Value + Alignment - 1u) & ~(Alignment - 1u);
}

static EFI_STATUS VirtualBlockReset(EFI_BLOCK_IO_PROTOCOL *This, BOOLEAN ExtendedVerification) {
    (void)This;
    (void)ExtendedVerification;
    return EFI_SUCCESS;
}

static EFI_STATUS VirtualBlockWrite(EFI_BLOCK_IO_PROTOCOL *This, UINT32 MediaId, UINT64 Lba, UINTN BufferSize, void *Buffer) {
    (void)This;
    (void)MediaId;
    (void)Lba;
    (void)BufferSize;
    (void)Buffer;
    return EFI_WRITE_PROTECTED;
}

static EFI_STATUS VirtualBlockFlush(EFI_BLOCK_IO_PROTOCOL *This) {
    (void)This;
    return EFI_SUCCESS;
}

static EFI_STATUS VirtualDiskWrite(EFI_DISK_IO_PROTOCOL *This, UINT32 MediaId, UINT64 Offset, UINTN BufferSize, void *Buffer) {
    (void)This;
    (void)MediaId;
    (void)Offset;
    (void)BufferSize;
    (void)Buffer;
    return EFI_WRITE_PROTECTED;
}

static EFI_STATUS MemoryBlockRead(EFI_BLOCK_IO_PROTOCOL *This, UINT32 MediaId, UINT64 Lba, UINTN BufferSize, void *Buffer) {
    MEMORY_BLOCK_CONTEXT *Ctx = (MEMORY_BLOCK_CONTEXT *)This;
    UINT64 Offset;
    UINT64 EndOffset;

    if (Ctx == 0 || Buffer == 0 || Ctx->Media.MediaPresent == 0) return EFI_INVALID_PARAMETER;
    if (MediaId != Ctx->Media.MediaId) return EFI_NOT_READY;
    if ((BufferSize % Ctx->Media.BlockSize) != 0u) return EFI_BAD_BUFFER_SIZE;
    if (BufferSize == 0) return EFI_SUCCESS;
    if (Lba > Ctx->Media.LastBlock) return EFI_INVALID_PARAMETER;
    if ((BufferSize / Ctx->Media.BlockSize) - 1u > Ctx->Media.LastBlock - Lba) return EFI_INVALID_PARAMETER;

    Offset = Lba * (UINT64)Ctx->Media.BlockSize;
    EndOffset = Offset + (UINT64)BufferSize;
    if (EndOffset > (UINT64)Ctx->ImageSize) return EFI_INVALID_PARAMETER;
    CopyBytes(Buffer, Ctx->Image + (UINTN)Offset, BufferSize);
    return EFI_SUCCESS;
}

static EFI_STATUS MemoryDiskRead(EFI_DISK_IO_PROTOCOL *This, UINT32 MediaId, UINT64 Offset, UINTN BufferSize, void *Buffer) {
    MEMORY_BLOCK_CONTEXT *Ctx = (MEMORY_BLOCK_CONTEXT *)((UINT8 *)This - CAYA_OFFSET_OF(MEMORY_BLOCK_CONTEXT, DiskIo));

    if (Ctx == 0 || Buffer == 0 || Ctx->Media.MediaPresent == 0) return EFI_INVALID_PARAMETER;
    if (MediaId != Ctx->Media.MediaId) return EFI_NOT_READY;
    if (Offset + (UINT64)BufferSize > (UINT64)Ctx->ImageSize) return EFI_INVALID_PARAMETER;
    CopyBytes(Buffer, Ctx->Image + (UINTN)Offset, BufferSize);
    return EFI_SUCCESS;
}

static EFI_STATUS IsoCdReadBytes(ISO_CD_CONTEXT *Ctx, UINT64 Offset, UINTN BufferSize, void *Buffer) {
    UINTN Readable;
    UINTN Tail;
    EFI_STATUS Status;

    if (Ctx == 0 || Buffer == 0) return EFI_INVALID_PARAMETER;
    if (BufferSize == 0) return EFI_SUCCESS;
    if (Offset >= Ctx->ImageSize) {
        Zero(Buffer, BufferSize);
        return EFI_SUCCESS;
    }

    Readable = (UINTN)(Ctx->ImageSize - Offset);
    if (Readable > BufferSize) Readable = BufferSize;
    Status = IsoReadBytes(&Ctx->Reader, Offset, Readable, Buffer);
    if (EFI_ERROR(Status)) return Status;

    Tail = BufferSize - Readable;
    if (Tail > 0) {
        Zero((UINT8 *)Buffer + Readable, Tail);
    }
    return EFI_SUCCESS;
}

static EFI_STATUS IsoCdBlockRead(EFI_BLOCK_IO_PROTOCOL *This, UINT32 MediaId, UINT64 Lba, UINTN BufferSize, void *Buffer) {
    ISO_CD_CONTEXT *Ctx = (ISO_CD_CONTEXT *)This;
    UINT64 BlockCount;

    if (Ctx == 0 || Buffer == 0 || Ctx->Media.MediaPresent == 0) return EFI_INVALID_PARAMETER;
    if (MediaId != Ctx->Media.MediaId) return EFI_NOT_READY;
    if ((BufferSize % Ctx->Media.BlockSize) != 0u) return EFI_BAD_BUFFER_SIZE;
    if (BufferSize == 0) return EFI_SUCCESS;
    BlockCount = (UINT64)(BufferSize / Ctx->Media.BlockSize);
    if (Lba > Ctx->Media.LastBlock || BlockCount - 1u > Ctx->Media.LastBlock - Lba) return EFI_INVALID_PARAMETER;
    return IsoCdReadBytes(Ctx, Lba * (UINT64)Ctx->Media.BlockSize, BufferSize, Buffer);
}

static EFI_STATUS IsoCdDiskRead(EFI_DISK_IO_PROTOCOL *This, UINT32 MediaId, UINT64 Offset, UINTN BufferSize, void *Buffer) {
    ISO_CD_CONTEXT *Ctx = (ISO_CD_CONTEXT *)((UINT8 *)This - CAYA_OFFSET_OF(ISO_CD_CONTEXT, DiskIo));

    if (Ctx == 0 || Buffer == 0 || Ctx->Media.MediaPresent == 0) return EFI_INVALID_PARAMETER;
    if (MediaId != Ctx->Media.MediaId) return EFI_NOT_READY;
    return IsoCdReadBytes(Ctx, Offset, BufferSize, Buffer);
}

static void InitEndDevicePath(EFI_DEVICE_PATH_PROTOCOL *Node) {
    Node->Type = END_DEVICE_PATH_TYPE;
    Node->SubType = END_ENTIRE_DEVICE_PATH_SUBTYPE;
    SetDevicePathLength(Node, (UINT16)sizeof(EFI_DEVICE_PATH_PROTOCOL));
}

static EFI_STATUS InstallVirtualBlockProtocols(EFI_HANDLE *Handle,
                                               EFI_DEVICE_PATH_PROTOCOL *DevicePath,
                                               EFI_BLOCK_IO_PROTOCOL *BlockIo,
                                               EFI_DISK_IO_PROTOCOL *DiskIo) {
    EFI_STATUS Status;

    Status = gBS->InstallProtocolInterface(Handle, &DevicePathProtocolGuid, EFI_NATIVE_INTERFACE, DevicePath);
    if (EFI_ERROR(Status)) return Status;
    Status = gBS->InstallProtocolInterface(Handle, &BlockIoProtocolGuid, EFI_NATIVE_INTERFACE, BlockIo);
    if (EFI_ERROR(Status)) return Status;
    Status = gBS->InstallProtocolInterface(Handle, &DiskIoProtocolGuid, EFI_NATIVE_INTERFACE, DiskIo);
    if (EFI_ERROR(Status)) return Status;

    (void)gBS->ConnectController(*Handle, 0, 0, 1);
    return EFI_SUCCESS;
}

static EFI_STATUS BuildMemoryFileDevicePath(MEMORY_BLOCK_CONTEXT *Ctx,
                                            const CHAR16 *FilePath,
                                            EFI_DEVICE_PATH_PROTOCOL **OutPath) {
    UINTN PathChars;
    UINTN FileNodeLen;
    UINTN ParentLen;
    UINTN TotalLen;
    UINT8 *Raw;
    EFI_DEVICE_PATH_PROTOCOL *FileNode;
    CHAR16 *PathText;
    EFI_DEVICE_PATH_PROTOCOL *EndNode;
    EFI_STATUS Status;

    *OutPath = 0;
    PathChars = StrLen(FilePath) + 1u;
    FileNodeLen = sizeof(EFI_DEVICE_PATH_PROTOCOL) + PathChars * sizeof(CHAR16);
    ParentLen = sizeof(CAYA_VENDOR_DEVICE_PATH) + sizeof(CAYA_CDROM_DEVICE_PATH);
    TotalLen = ParentLen + FileNodeLen + sizeof(EFI_DEVICE_PATH_PROTOCOL);
    if (FileNodeLen > 0xFFFFu || TotalLen > 0xFFFFu) return EFI_INVALID_PARAMETER;

    Status = gBS->AllocatePool(EFI_LOADER_DATA, TotalLen, (void **)&Raw);
    if (EFI_ERROR(Status)) return Status;
    Zero(Raw, TotalLen);

    CopyBytes(Raw, &Ctx->DevicePath.Vendor, ParentLen);
    FileNode = (EFI_DEVICE_PATH_PROTOCOL *)(Raw + ParentLen);
    FileNode->Type = MEDIA_DEVICE_PATH;
    FileNode->SubType = MEDIA_FILEPATH_DP;
    SetDevicePathLength(FileNode, (UINT16)FileNodeLen);
    PathText = (CHAR16 *)(Raw + ParentLen + sizeof(EFI_DEVICE_PATH_PROTOCOL));
    CopyBytes(PathText, FilePath, PathChars * sizeof(CHAR16));
    EndNode = (EFI_DEVICE_PATH_PROTOCOL *)(Raw + ParentLen + FileNodeLen);
    InitEndDevicePath(EndNode);

    *OutPath = (EFI_DEVICE_PATH_PROTOCOL *)Raw;
    return EFI_SUCCESS;
}

static EFI_STATUS ProbeSimpleFile(EFI_HANDLE Handle, const CHAR16 *Path) {
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *Fs = 0;
    EFI_FILE_PROTOCOL *Root = 0;
    EFI_FILE_PROTOCOL *File = 0;
    EFI_STATUS Status;

    Status = gBS->HandleProtocol(Handle, &SimpleFileSystemProtocolGuid, (void **)&Fs);
    if (EFI_ERROR(Status) || Fs == 0) return Status;

    Status = Fs->OpenVolume(Fs, &Root);
    if (EFI_ERROR(Status) || Root == 0) return Status;

    Status = Root->Open(Root, &File, (CHAR16 *)Path, EFI_FILE_MODE_READ, 0);
    if (!EFI_ERROR(Status) && File != 0) {
        File->Close(File);
    }
    Root->Close(Root);
    return Status;
}

/* Forward declaration — implementation lives later in the file. */
static EFI_STATUS IsoFindEntry(ISO_READER *R, UINT32 DirLba, UINT32 DirLen,
                               const char *Name, BOOLEAN IsDir,
                               UINT32 *OutLba, UINT32 *OutLen);

/* ----------------------------------------------------------------------
   Minimal read-only ISO9660 EFI_SIMPLE_FILE_SYSTEM_PROTOCOL implementation.

   Installed on the virtual ISO CD handle so that chainloaded EFI images
   (Windows boot manager, Linux loaders) can open files through their
   LoadedImage->DeviceHandle even when the firmware does not provide a
   native ISO9660 driver. This is the core of the Ventoy approach: the
   loaded image sees a real filesystem on its install media handle, so
   Windows installer finds \sources\boot.wim, \sources\install.wim, etc.
   on the same device that BOOTX64.EFI was loaded from.
   ---------------------------------------------------------------------- */

#define CAYA_ISOFILE_SIG  0xCA7A1503u
#define CAYA_ISOFS_SIG    0xCA7A1504u
#define EFI_FILE_PROTOCOL_REVISION 0x00010000ULL

typedef struct CAYA_ISOFS_S CAYA_ISOFS;

typedef struct {
    EFI_FILE_PROTOCOL Proto;
    UINT32 Sig;
    CAYA_ISOFS *Fs;
    UINT32 ExtentLba;
    UINT32 ExtentLen;
    UINT8  IsDir;
    UINT64 Position;
    UINT8  *DirBuf;
    UINT32 DirBufSize;
    UINT32 DirCursor;
} CAYA_ISOFILE;

struct CAYA_ISOFS_S {
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL Proto;
    UINT32 Sig;
    ISO_READER Reader;
    UINT32 RootLba;
    UINT32 RootLen;
};

static EFI_STATUS IsoFileOpen(EFI_FILE_PROTOCOL *This, EFI_FILE_PROTOCOL **NewHandle,
                               CHAR16 *FileName, UINT64 OpenMode, UINT64 Attributes);
static EFI_STATUS IsoFileClose(EFI_FILE_PROTOCOL *This);
static EFI_STATUS IsoFileRead(EFI_FILE_PROTOCOL *This, UINTN *BufferSize, void *Buffer);
static EFI_STATUS IsoFileGetPosition(EFI_FILE_PROTOCOL *This, UINT64 *Position);
static EFI_STATUS IsoFileSetPosition(EFI_FILE_PROTOCOL *This, UINT64 Position);
static EFI_STATUS IsoFileGetInfo(EFI_FILE_PROTOCOL *This, EFI_GUID *Type, UINTN *BufferSize, void *Buffer);

static CAYA_ISOFILE *IsoMakeNode(CAYA_ISOFS *Fs, UINT32 Lba, UINT32 Len, UINT8 IsDir) {
    CAYA_ISOFILE *N;
    if (EFI_ERROR(gBS->AllocatePool(EFI_LOADER_DATA, sizeof(*N), (void **)&N))) return 0;
    Zero(N, sizeof(*N));
    N->Sig = CAYA_ISOFILE_SIG;
    N->Fs = Fs;
    N->ExtentLba = Lba;
    N->ExtentLen = Len;
    N->IsDir = IsDir;
    N->Position = 0;
    N->Proto.Revision    = EFI_FILE_PROTOCOL_REVISION;
    N->Proto.Open        = IsoFileOpen;
    N->Proto.Close       = IsoFileClose;
    N->Proto.Read        = IsoFileRead;
    N->Proto.GetPosition = IsoFileGetPosition;
    N->Proto.SetPosition = IsoFileSetPosition;
    N->Proto.GetInfo     = IsoFileGetInfo;
    return N;
}

static EFI_STATUS IsoFsOpenVolume(void *This, EFI_FILE_PROTOCOL **Root) {
    CAYA_ISOFS *Fs = (CAYA_ISOFS *)This;
    CAYA_ISOFILE *N;
    if (Fs == 0 || Fs->Sig != CAYA_ISOFS_SIG || Root == 0) return EFI_INVALID_PARAMETER;
    N = IsoMakeNode(Fs, Fs->RootLba, Fs->RootLen, 1);
    if (N == 0) return EFI_OUT_OF_RESOURCES;
    *Root = &N->Proto;
    return EFI_SUCCESS;
}

static EFI_STATUS IsoFileOpen(EFI_FILE_PROTOCOL *This, EFI_FILE_PROTOCOL **NewHandle,
                               CHAR16 *FileName, UINT64 OpenMode, UINT64 Attributes) {
    CAYA_ISOFILE *Cur = (CAYA_ISOFILE *)This;
    CAYA_ISOFS *Fs;
    CHAR16 Comp[MAX_NAME];
    char   Ascii[MAX_NAME];
    UINTN  i, j;
    UINT32 CurLba, CurLen;
    UINT8  CurIsDir;

    (void)OpenMode; (void)Attributes;
    if (Cur == 0 || Cur->Sig != CAYA_ISOFILE_SIG || NewHandle == 0 || FileName == 0)
        return EFI_INVALID_PARAMETER;
    Fs = Cur->Fs;

    if (FileName[0] == 0) {
        CAYA_ISOFILE *N = IsoMakeNode(Fs, Cur->ExtentLba, Cur->ExtentLen, Cur->IsDir);
        if (N == 0) return EFI_OUT_OF_RESOURCES;
        *NewHandle = &N->Proto;
        return EFI_SUCCESS;
    }

    if (FileName[0] == L'\\' || FileName[0] == L'/') {
        CurLba = Fs->RootLba; CurLen = Fs->RootLen; CurIsDir = 1;
    } else {
        CurLba = Cur->ExtentLba; CurLen = Cur->ExtentLen; CurIsDir = Cur->IsDir;
    }

    i = 0;
    while (FileName[i] != 0) {
        while (FileName[i] == L'\\' || FileName[i] == L'/') i++;
        if (FileName[i] == 0) break;

        j = 0;
        while (FileName[i] != 0 && FileName[i] != L'\\' && FileName[i] != L'/' &&
               j + 1 < MAX_NAME) {
            Comp[j++] = FileName[i++];
        }
        Comp[j] = 0;

        if (Comp[0] == L'.' && Comp[1] == 0) continue;
        if (Comp[0] == L'.' && Comp[1] == L'.' && Comp[2] == 0) {
            /* Parent traversal not tracked; restart from root. */
            CurLba = Fs->RootLba; CurLen = Fs->RootLen; CurIsDir = 1;
            continue;
        }

        if (!CurIsDir) return EFI_NOT_FOUND;

        /* Convert UCS-2 component to ASCII (uppercased; ISO9660 8.3 names are
           upper-case ASCII). Non-ASCII characters fall back to '?'. */
        {
            UINTN k;
            for (k = 0; k < j; k++) {
                CHAR16 c = Comp[k];
                if (c >= 0x80) c = L'?';
                Ascii[k] = (char)c;
            }
            Ascii[k] = 0;
        }

        {
            UINT32 Lba, Len;
            EFI_STATUS St;
            UINT8 IsDir;

            St = IsoFindEntry(&Fs->Reader, CurLba, CurLen, Ascii, 1, &Lba, &Len);
            if (!EFI_ERROR(St)) { IsDir = 1; }
            else {
                St = IsoFindEntry(&Fs->Reader, CurLba, CurLen, Ascii, 0, &Lba, &Len);
                if (EFI_ERROR(St)) return EFI_NOT_FOUND;
                IsDir = 0;
            }
            CurLba = Lba; CurLen = Len; CurIsDir = IsDir;
        }
    }

    {
        CAYA_ISOFILE *N = IsoMakeNode(Fs, CurLba, CurLen, CurIsDir);
        if (N == 0) return EFI_OUT_OF_RESOURCES;
        *NewHandle = &N->Proto;
    }
    return EFI_SUCCESS;
}

static EFI_STATUS IsoFileClose(EFI_FILE_PROTOCOL *This) {
    CAYA_ISOFILE *N = (CAYA_ISOFILE *)This;
    if (N == 0 || N->Sig != CAYA_ISOFILE_SIG) return EFI_INVALID_PARAMETER;
    if (N->DirBuf) gBS->FreePool(N->DirBuf);
    N->Sig = 0;
    gBS->FreePool(N);
    return EFI_SUCCESS;
}

static EFI_STATUS IsoFileReadFileBytes(CAYA_ISOFILE *N, UINTN *BufferSize, void *Buffer) {
    UINTN Want = *BufferSize;
    UINT64 Avail;
    if (N->Position >= (UINT64)N->ExtentLen) {
        *BufferSize = 0;
        return EFI_SUCCESS;
    }
    Avail = (UINT64)N->ExtentLen - N->Position;
    if ((UINT64)Want > Avail) Want = (UINTN)Avail;
    if (Want == 0) {
        *BufferSize = 0;
        return EFI_SUCCESS;
    }
    {
        UINT64 IsoOffset = (UINT64)N->ExtentLba * (UINT64)ISO_SECTOR_SIZE + N->Position;
        EFI_STATUS Status = IsoReadBytes(&N->Fs->Reader, IsoOffset, Want, Buffer);
        if (EFI_ERROR(Status)) return Status;
        N->Position += Want;
        *BufferSize = Want;
    }
    return EFI_SUCCESS;
}

static EFI_STATUS IsoFileReadDirEntry(CAYA_ISOFILE *N, UINTN *BufferSize, void *Buffer) {
    if (N->DirBuf == 0) {
        UINT32 SecCnt = (N->ExtentLen + ISO_SECTOR_SIZE - 1u) / ISO_SECTOR_SIZE;
        EFI_STATUS Status;
        if (SecCnt == 0) SecCnt = 1;
        if (SecCnt > 64u) SecCnt = 64u;
        N->DirBufSize = SecCnt * ISO_SECTOR_SIZE;
        Status = gBS->AllocatePool(EFI_LOADER_DATA, N->DirBufSize, (void **)&N->DirBuf);
        if (EFI_ERROR(Status)) { N->DirBuf = 0; return Status; }
        Status = IsoRead(&N->Fs->Reader, N->ExtentLba, SecCnt, N->DirBuf);
        if (EFI_ERROR(Status)) { gBS->FreePool(N->DirBuf); N->DirBuf = 0; return Status; }
        N->DirCursor = 0;
    }

    while (N->DirCursor < N->ExtentLen && N->DirCursor + 33u <= N->DirBufSize) {
        UINT8 RecLen = N->DirBuf[N->DirCursor];
        if (RecLen == 0) {
            UINT32 Next = ((N->DirCursor / ISO_SECTOR_SIZE) + 1u) * ISO_SECTOR_SIZE;
            if (Next >= N->ExtentLen) break;
            N->DirCursor = Next;
            continue;
        }
        if (RecLen < 33u || N->DirCursor + RecLen > N->DirBufSize) break;
        {
            UINT8  IdLen   = N->DirBuf[N->DirCursor + 32];
            UINT8 *Id      = N->DirBuf + N->DirCursor + 33;
            UINT8  Flags   = N->DirBuf[N->DirCursor + 25];
            UINT32 ExtLba  = (UINT32)N->DirBuf[N->DirCursor+2] |
                             ((UINT32)N->DirBuf[N->DirCursor+3]<<8) |
                             ((UINT32)N->DirBuf[N->DirCursor+4]<<16) |
                             ((UINT32)N->DirBuf[N->DirCursor+5]<<24);
            UINT32 ExtLen  = (UINT32)N->DirBuf[N->DirCursor+10] |
                             ((UINT32)N->DirBuf[N->DirCursor+11]<<8) |
                             ((UINT32)N->DirBuf[N->DirCursor+12]<<16) |
                             ((UINT32)N->DirBuf[N->DirCursor+13]<<24);
            BOOLEAN IsSub  = (Flags & 0x02u) != 0;
            UINT8 NameLen;
            UINTN InfoSize;
            EFI_FILE_INFO *Info;
            UINTN k;

            if (IdLen == 1 && (Id[0] == 0x00u || Id[0] == 0x01u)) {
                N->DirCursor += RecLen;
                continue;
            }

            NameLen = IdLen;
            if (NameLen >= 2 && Id[NameLen-2] == ';' && Id[NameLen-1] == '1') NameLen -= 2;
            if (NameLen > 0 && Id[NameLen-1] == '.') NameLen--; /* trailing dot */
            (void)ExtLba;

            InfoSize = sizeof(EFI_FILE_INFO) + ((UINTN)NameLen + 1u) * sizeof(CHAR16);
            if (*BufferSize < InfoSize) {
                *BufferSize = InfoSize;
                return EFI_BUFFER_TOO_SMALL;
            }

            Info = (EFI_FILE_INFO *)Buffer;
            Zero(Info, InfoSize);
            Info->Size         = InfoSize;
            Info->FileSize     = IsSub ? 0 : ExtLen;
            Info->PhysicalSize = Info->FileSize;
            Info->Attribute    = (IsSub ? EFI_FILE_DIRECTORY : 0) | 0x01u; /* read-only */
            for (k = 0; k < NameLen; k++) {
                CHAR16 c = (CHAR16)Id[k];
                if (c >= L'A' && c <= L'Z') c = (CHAR16)(c + (L'a' - L'A'));
                Info->FileName[k] = c;
            }
            Info->FileName[NameLen] = 0;

            *BufferSize = InfoSize;
            N->DirCursor += RecLen;
            return EFI_SUCCESS;
        }
    }

    *BufferSize = 0;
    return EFI_SUCCESS;
}

static EFI_STATUS IsoFileRead(EFI_FILE_PROTOCOL *This, UINTN *BufferSize, void *Buffer) {
    CAYA_ISOFILE *N = (CAYA_ISOFILE *)This;
    if (N == 0 || N->Sig != CAYA_ISOFILE_SIG || BufferSize == 0)
        return EFI_INVALID_PARAMETER;
    if (N->IsDir) return IsoFileReadDirEntry(N, BufferSize, Buffer);
    return IsoFileReadFileBytes(N, BufferSize, Buffer);
}

static EFI_STATUS IsoFileGetPosition(EFI_FILE_PROTOCOL *This, UINT64 *Position) {
    CAYA_ISOFILE *N = (CAYA_ISOFILE *)This;
    if (N == 0 || N->Sig != CAYA_ISOFILE_SIG || Position == 0) return EFI_INVALID_PARAMETER;
    if (N->IsDir) return EFI_UNSUPPORTED;
    *Position = N->Position;
    return EFI_SUCCESS;
}

static EFI_STATUS IsoFileSetPosition(EFI_FILE_PROTOCOL *This, UINT64 Position) {
    CAYA_ISOFILE *N = (CAYA_ISOFILE *)This;
    if (N == 0 || N->Sig != CAYA_ISOFILE_SIG) return EFI_INVALID_PARAMETER;
    if (N->IsDir) {
        if (Position != 0) return EFI_UNSUPPORTED;
        N->DirCursor = 0;
        if (N->DirBuf) { gBS->FreePool(N->DirBuf); N->DirBuf = 0; N->DirBufSize = 0; }
        return EFI_SUCCESS;
    }
    if (Position == 0xFFFFFFFFFFFFFFFFULL) {
        N->Position = N->ExtentLen;
    } else {
        N->Position = Position;
    }
    return EFI_SUCCESS;
}

static BOOLEAN GuidEqual(const EFI_GUID *A, const EFI_GUID *B) {
    UINTN i;
    if (A->Data1 != B->Data1) return 0;
    if (A->Data2 != B->Data2) return 0;
    if (A->Data3 != B->Data3) return 0;
    for (i = 0; i < 8; i++) if (A->Data4[i] != B->Data4[i]) return 0;
    return 1;
}

static EFI_STATUS IsoFileGetInfo(EFI_FILE_PROTOCOL *This, EFI_GUID *Type,
                                  UINTN *BufferSize, void *Buffer) {
    CAYA_ISOFILE *N = (CAYA_ISOFILE *)This;
    UINTN InfoSize;
    EFI_FILE_INFO *Info;

    if (N == 0 || N->Sig != CAYA_ISOFILE_SIG || Type == 0 || BufferSize == 0)
        return EFI_INVALID_PARAMETER;
    if (!GuidEqual(Type, &FileInfoGuid)) return EFI_UNSUPPORTED;

    InfoSize = sizeof(EFI_FILE_INFO) + sizeof(CHAR16);
    if (*BufferSize < InfoSize) {
        *BufferSize = InfoSize;
        return EFI_BUFFER_TOO_SMALL;
    }
    Info = (EFI_FILE_INFO *)Buffer;
    Zero(Info, InfoSize);
    Info->Size         = InfoSize;
    Info->FileSize     = N->IsDir ? 0 : N->ExtentLen;
    Info->PhysicalSize = Info->FileSize;
    Info->Attribute    = (N->IsDir ? EFI_FILE_DIRECTORY : 0) | 0x01u;
    Info->FileName[0]  = 0;
    *BufferSize        = InfoSize;
    return EFI_SUCCESS;
}

/* Allocate and install our SimpleFileSystem on the given handle, reading
   the root directory location from the ISO PVD. Reader is copied (it
   contains a file handle pointer that the caller must keep alive while
   the SimpleFs is in use).

   If the firmware already installed a SimpleFileSystem on the handle
   (because it has its own ISO9660 driver), we leave that alone — both
   work; the firmware version is just as fine. */
static EFI_STATUS InstallIsoSimpleFs(EFI_HANDLE Handle, ISO_READER *Reader) {
    CAYA_ISOFS *Fs;
    EFI_STATUS Status;
    UINT8 Pvd[ISO_SECTOR_SIZE];
    void *Existing = 0;

    if (Handle == 0 || Reader == 0) return EFI_INVALID_PARAMETER;

    if (!EFI_ERROR(gBS->HandleProtocol(Handle, &SimpleFileSystemProtocolGuid, &Existing))
        && Existing != 0) {
        /* Firmware already attached its ISO9660 driver. */
        return EFI_SUCCESS;
    }

    Status = IsoRead(Reader, ISO_PVD_LBA, 1, Pvd);
    if (EFI_ERROR(Status)) return Status;
    if (Pvd[0] != 1 || Pvd[1] != 'C' || Pvd[2] != 'D' ||
        Pvd[3] != '0' || Pvd[4] != '0' || Pvd[5] != '1') {
        return EFI_LOAD_ERROR;
    }

    Status = gBS->AllocatePool(EFI_LOADER_DATA, sizeof(*Fs), (void **)&Fs);
    if (EFI_ERROR(Status)) return Status;
    Zero(Fs, sizeof(*Fs));
    Fs->Sig    = CAYA_ISOFS_SIG;
    Fs->Reader = *Reader;
    Fs->RootLba = (UINT32)Pvd[158] | ((UINT32)Pvd[159]<<8) |
                  ((UINT32)Pvd[160]<<16) | ((UINT32)Pvd[161]<<24);
    Fs->RootLen = (UINT32)Pvd[166] | ((UINT32)Pvd[167]<<8) |
                  ((UINT32)Pvd[168]<<16) | ((UINT32)Pvd[169]<<24);
    Fs->Proto.Revision   = 0x00010000ULL;
    Fs->Proto.OpenVolume = IsoFsOpenVolume;

    Status = gBS->InstallProtocolInterface(&Handle, &SimpleFileSystemProtocolGuid,
                                           EFI_NATIVE_INTERFACE, &Fs->Proto);
    if (EFI_ERROR(Status)) {
        gBS->FreePool(Fs);
        return Status;
    }
    return EFI_SUCCESS;
}

static EFI_STATUS InstallIsoVirtualCd(ISO_READER *Reader, UINT64 IsoSizeBytes,
                                       EFI_HANDLE *OutHandle) {
    ISO_CD_CONTEXT *Ctx;
    EFI_HANDLE Handle = 0;
    UINT64 Blocks;
    EFI_STATUS Status;

    if (OutHandle != 0) *OutHandle = 0;
    if (Reader == 0 || IsoSizeBytes < ISO_SECTOR_SIZE) return EFI_INVALID_PARAMETER;

    Status = gBS->AllocatePool(EFI_LOADER_DATA, sizeof(ISO_CD_CONTEXT), (void **)&Ctx);
    if (EFI_ERROR(Status)) return Status;
    Zero(Ctx, sizeof(ISO_CD_CONTEXT));

    Ctx->Reader = *Reader;
    Ctx->ImageSize = IsoSizeBytes;
    Blocks = (IsoSizeBytes + ISO_SECTOR_SIZE - 1u) / ISO_SECTOR_SIZE;

    Ctx->Media.MediaId = 0xCA7A1501u;
    Ctx->Media.RemovableMedia = 1;
    Ctx->Media.MediaPresent = 1;
    Ctx->Media.LogicalPartition = 0;
    Ctx->Media.ReadOnly = 1;
    Ctx->Media.WriteCaching = 0;
    Ctx->Media.BlockSize = ISO_SECTOR_SIZE;
    Ctx->Media.IoAlign = 1;
    Ctx->Media.LastBlock = Blocks > 0 ? Blocks - 1u : 0;

    Ctx->BlockIo.Revision = EFI_BLOCK_IO_PROTOCOL_REVISION;
    Ctx->BlockIo.Media = &Ctx->Media;
    Ctx->BlockIo.Reset = VirtualBlockReset;
    Ctx->BlockIo.ReadBlocks = IsoCdBlockRead;
    Ctx->BlockIo.WriteBlocks = VirtualBlockWrite;
    Ctx->BlockIo.FlushBlocks = VirtualBlockFlush;
    Ctx->DiskIo.Revision = EFI_DISK_IO_PROTOCOL_REVISION;
    Ctx->DiskIo.ReadDisk = IsoCdDiskRead;
    Ctx->DiskIo.WriteDisk = VirtualDiskWrite;

    Ctx->DevicePath.Vendor.Header.Type = MEDIA_DEVICE_PATH;
    Ctx->DevicePath.Vendor.Header.SubType = MEDIA_VENDOR_DP;
    SetDevicePathLength(&Ctx->DevicePath.Vendor.Header, (UINT16)sizeof(CAYA_VENDOR_DEVICE_PATH));
    Ctx->DevicePath.Vendor.Guid = CayaRamDiskVendorGuid;
    InitEndDevicePath(&Ctx->DevicePath.End);

    Status = InstallVirtualBlockProtocols(&Handle,
                                          (EFI_DEVICE_PATH_PROTOCOL *)&Ctx->DevicePath,
                                          &Ctx->BlockIo,
                                          &Ctx->DiskIo);
    if (!EFI_ERROR(Status)) {
        /* Install our own ISO9660 SimpleFileSystem on the same handle. This
           is what makes Windows installer / Linux loaders see the ISO content
           via LoadedImage->DeviceHandle, regardless of whether the firmware
           ships an ISO9660 driver. */
        (void)InstallIsoSimpleFs(Handle, Reader);
        Ctx->Handle = Handle;
        if (OutHandle != 0) *OutHandle = Handle;
    }
    return Status;
}

/* Build a device path: <Parent device path> + FilePath(FilePath16) + End.
   Caller must FreePool the result. */
static EFI_STATUS BuildHandleFilePath(EFI_HANDLE Handle, const CHAR16 *FilePath16,
                                       EFI_DEVICE_PATH_PROTOCOL **OutPath) {
    EFI_DEVICE_PATH_PROTOCOL *Parent = 0;
    EFI_DEVICE_PATH_PROTOCOL *Walk;
    UINTN ParentLen;
    UINTN PathChars;
    UINTN FileNodeLen;
    UINTN TotalLen;
    UINT8 *Raw;
    EFI_DEVICE_PATH_PROTOCOL *FileNode;
    EFI_DEVICE_PATH_PROTOCOL *EndNode;
    CHAR16 *PathText;
    EFI_STATUS Status;

    *OutPath = 0;
    Status = gBS->HandleProtocol(Handle, &DevicePathProtocolGuid, (void **)&Parent);
    if (EFI_ERROR(Status) || Parent == 0) return EFI_NOT_FOUND;

    /* Measure parent length up to (but not including) the End node. */
    ParentLen = 0;
    Walk = Parent;
    while (Walk->Type != END_DEVICE_PATH_TYPE) {
        UINT16 Len = (UINT16)(Walk->Length[0] | ((UINT16)Walk->Length[1] << 8));
        if (Len < sizeof(EFI_DEVICE_PATH_PROTOCOL)) return EFI_INVALID_PARAMETER;
        ParentLen += Len;
        Walk = (EFI_DEVICE_PATH_PROTOCOL *)((UINT8 *)Walk + Len);
        if (ParentLen > 0x4000u) return EFI_INVALID_PARAMETER;
    }

    PathChars = StrLen(FilePath16) + 1u;
    FileNodeLen = sizeof(EFI_DEVICE_PATH_PROTOCOL) + PathChars * sizeof(CHAR16);
    TotalLen = ParentLen + FileNodeLen + sizeof(EFI_DEVICE_PATH_PROTOCOL);
    if (FileNodeLen > 0xFFFFu || TotalLen > 0xFFFFu) return EFI_INVALID_PARAMETER;

    Status = gBS->AllocatePool(EFI_LOADER_DATA, TotalLen, (void **)&Raw);
    if (EFI_ERROR(Status)) return Status;
    Zero(Raw, TotalLen);

    CopyBytes(Raw, Parent, ParentLen);

    FileNode = (EFI_DEVICE_PATH_PROTOCOL *)(Raw + ParentLen);
    FileNode->Type = MEDIA_DEVICE_PATH;
    FileNode->SubType = MEDIA_FILEPATH_DP;
    SetDevicePathLength(FileNode, (UINT16)FileNodeLen);
    PathText = (CHAR16 *)(Raw + ParentLen + sizeof(EFI_DEVICE_PATH_PROTOCOL));
    CopyBytes(PathText, FilePath16, PathChars * sizeof(CHAR16));

    EndNode = (EFI_DEVICE_PATH_PROTOCOL *)(Raw + ParentLen + FileNodeLen);
    InitEndDevicePath(EndNode);

    *OutPath = (EFI_DEVICE_PATH_PROTOCOL *)Raw;
    return EFI_SUCCESS;
}

/* Try to LoadImage \EFI\BOOT\BOOTX64.EFI through Handle's SimpleFileSystem.
   This is the Ventoy approach: BOOTX64.EFI's LoadedImage->DeviceHandle ends
   up pointing to the virtual ISO CD itself, so when Windows boot manager
   looks for \sources\boot.wim it finds it on the same device. */
static EFI_STATUS BootBootx64FromHandle(EFI_HANDLE Handle) {
    EFI_DEVICE_PATH_PROTOCOL *FilePath = 0;
    EFI_HANDLE Child = 0;
    EFI_STATUS Status;
    const CHAR16 *BootPath = L"\\EFI\\BOOT\\BOOTX64.EFI";

    if (Handle == 0) return EFI_INVALID_PARAMETER;

    /* Make sure the firmware has had a chance to attach an ISO9660 driver
       and install SimpleFileSystem on the virtual CD. */
    (void)gBS->ConnectController(Handle, 0, 0, 1);

    Status = ProbeSimpleFile(Handle, BootPath);
    if (EFI_ERROR(Status)) {
        return EFI_NOT_FOUND;
    }

    Status = BuildHandleFilePath(Handle, BootPath, &FilePath);
    if (EFI_ERROR(Status)) return Status;

    Status = gBS->LoadImage(1, gImageHandle, FilePath, 0, 0, &Child);
    gBS->FreePool(FilePath);
    if (EFI_ERROR(Status)) return Status;

    return gBS->StartImage(Child, 0, 0);
}

static EFI_STATUS BootFromFatImageDevice(UINT8 *Img, UINTN ImgSize, UINT32 BlockSize,
                                         UINT32 CdImageLba, UINT32 CdImageSectors,
                                         UINT32 BootEntryNumber) {
    MEMORY_BLOCK_CONTEXT *Ctx;
    EFI_HANDLE Handle = 0;
    EFI_DEVICE_PATH_PROTOCOL *FilePath = 0;
    EFI_HANDLE Child = 0;
    UINTN StoredSize;
    EFI_STATUS Status;
    const CHAR16 *BootPath = L"\\EFI\\BOOT\\BOOTX64.EFI";

    if (Img == 0 || ImgSize < 512u) return EFI_INVALID_PARAMETER;
    if (BlockSize < 512u || BlockSize > 4096u || (BlockSize & (BlockSize - 1u)) != 0u) {
        BlockSize = 512u;
    }
    StoredSize = RoundUpU(ImgSize, BlockSize);
    if (StoredSize == 0 || StoredSize / BlockSize == 0) return EFI_INVALID_PARAMETER;

    Status = gBS->AllocatePool(EFI_LOADER_DATA, sizeof(MEMORY_BLOCK_CONTEXT), (void **)&Ctx);
    if (EFI_ERROR(Status)) return Status;
    Zero(Ctx, sizeof(MEMORY_BLOCK_CONTEXT));

    Status = gBS->AllocatePool(EFI_LOADER_DATA, StoredSize, (void **)&Ctx->Image);
    if (EFI_ERROR(Status)) return Status;
    Zero(Ctx->Image, StoredSize);
    CopyBytes(Ctx->Image, Img, ImgSize);
    Ctx->ImageSize = StoredSize;

    Ctx->Media.MediaId = 0xCA7A1502u;
    Ctx->Media.RemovableMedia = 1;
    Ctx->Media.MediaPresent = 1;
    Ctx->Media.LogicalPartition = 1;
    Ctx->Media.ReadOnly = 1;
    Ctx->Media.WriteCaching = 0;
    Ctx->Media.BlockSize = BlockSize;
    Ctx->Media.IoAlign = 1;
    Ctx->Media.LastBlock = (StoredSize / BlockSize) - 1u;

    Ctx->BlockIo.Revision = EFI_BLOCK_IO_PROTOCOL_REVISION;
    Ctx->BlockIo.Media = &Ctx->Media;
    Ctx->BlockIo.Reset = VirtualBlockReset;
    Ctx->BlockIo.ReadBlocks = MemoryBlockRead;
    Ctx->BlockIo.WriteBlocks = VirtualBlockWrite;
    Ctx->BlockIo.FlushBlocks = VirtualBlockFlush;
    Ctx->DiskIo.Revision = EFI_DISK_IO_PROTOCOL_REVISION;
    Ctx->DiskIo.ReadDisk = MemoryDiskRead;
    Ctx->DiskIo.WriteDisk = VirtualDiskWrite;

    Ctx->DevicePath.Vendor.Header.Type = MEDIA_DEVICE_PATH;
    Ctx->DevicePath.Vendor.Header.SubType = MEDIA_VENDOR_DP;
    SetDevicePathLength(&Ctx->DevicePath.Vendor.Header, (UINT16)sizeof(CAYA_VENDOR_DEVICE_PATH));
    Ctx->DevicePath.Vendor.Guid = CayaRamDiskVendorGuid;

    Ctx->DevicePath.Cdrom.Header.Type = MEDIA_DEVICE_PATH;
    Ctx->DevicePath.Cdrom.Header.SubType = MEDIA_CDROM_DP;
    SetDevicePathLength(&Ctx->DevicePath.Cdrom.Header, (UINT16)sizeof(CAYA_CDROM_DEVICE_PATH));
    Ctx->DevicePath.Cdrom.BootEntry = BootEntryNumber;
    Ctx->DevicePath.Cdrom.PartitionStart = CdImageLba;
    Ctx->DevicePath.Cdrom.PartitionSize = CdImageSectors;
    InitEndDevicePath(&Ctx->DevicePath.End);

    Status = InstallVirtualBlockProtocols(&Handle,
                                          (EFI_DEVICE_PATH_PROTOCOL *)&Ctx->DevicePath,
                                          &Ctx->BlockIo,
                                          &Ctx->DiskIo);
    if (EFI_ERROR(Status)) {
        return IsoFail(Status, L"Could not expose El Torito FAT image as an EFI volume.");
    }

    Status = ProbeSimpleFile(Handle, BootPath);
    if (EFI_ERROR(Status)) {
        return IsoFail(Status, L"Firmware FAT driver could not mount the El Torito image.");
    }

    Status = BuildMemoryFileDevicePath(Ctx, BootPath, &FilePath);
    if (EFI_ERROR(Status)) return Status;

    Status = gBS->LoadImage(1, gImageHandle, FilePath, 0, 0, &Child);
    gBS->FreePool(FilePath);
    if (EFI_ERROR(Status)) {
        return IsoFail(Status, L"LoadImage failed from mounted El Torito EFI volume.");
    }

    Status = gBS->StartImage(Child, 0, 0);
    if (EFI_ERROR(Status)) {
        return IsoFail(Status, L"UEFI image returned an error after virtual CD/DVD mapping.");
    }
    return Status;
}

/* Case-insensitive ASCII compare against an ISO 9660 identifier.
   The ISO identifier may have a ";1" version suffix which is stripped. */
static BOOLEAN IsoNameEq(const UINT8 *IsoId, UINT8 IdLen, const char *Target) {
    UINT8 Len = IdLen;
    UINTN TLen = 0;
    UINTN i;
    const char *p = Target;
    while (*p++) TLen++;

    if (Len >= 2 && IsoId[Len-2] == ';' && IsoId[Len-1] == '1') Len -= 2;
    if (Len != (UINT8)TLen) return 0;

    for (i = 0; i < Len; i++) {
        UINT8 a = IsoId[i];
        UINT8 b = (UINT8)Target[i];
        if (a >= 'a' && a <= 'z') a -= 32;
        if (b >= 'a' && b <= 'z') b -= 32;
        if (a != b) return 0;
    }
    return 1;
}

/* Walk one ISO 9660 directory extent to find a named entry. */
static EFI_STATUS IsoFindEntry(ISO_READER *R,
                                UINT32 DirLba, UINT32 DirLen,
                                const char *Name, BOOLEAN IsDir,
                                UINT32 *OutLba, UINT32 *OutLen)
{
    EFI_STATUS Status;
    UINT32  SecCnt = (DirLen + ISO_SECTOR_SIZE - 1) / ISO_SECTOR_SIZE;
    UINT8  *Buf;
    UINT32  Off = 0;

    /* Always read at least 1 sector; guard against DirLen=0 */
    if (SecCnt < 1u) SecCnt = 1u;
    /* Cap at 64 sectors (128KB) — no reasonable ISO directory is larger */
    if (SecCnt > 64u) SecCnt = 64u;

    Status = gBS->AllocatePool(EFI_LOADER_DATA, SecCnt * ISO_SECTOR_SIZE, (void **)&Buf);
    if (EFI_ERROR(Status)) return Status;

    Status = IsoRead(R, DirLba, SecCnt, Buf);
    if (EFI_ERROR(Status)) { gBS->FreePool(Buf); return Status; }

    Status = EFI_NOT_FOUND;
    {
        UINT32 ScanLen = (DirLen > 0) ? DirLen : SecCnt * ISO_SECTOR_SIZE;
    while (Off < ScanLen) {
        UINT8 RecLen = Buf[Off];
        if (RecLen == 0) {
            UINT32 Next = ((Off / ISO_SECTOR_SIZE) + 1) * ISO_SECTOR_SIZE;
            if (Next >= ScanLen) break;
            Off = Next;
            continue;
        }
        {
            UINT8  IdLen    = Buf[Off + 32];
            UINT8 *Id       = Buf + Off + 33;
            UINT8  Flags    = Buf[Off + 25];
            UINT32 ExtLba   = (UINT32)Buf[Off+2]  | ((UINT32)Buf[Off+3] <<8) |
                              ((UINT32)Buf[Off+4]<<16) | ((UINT32)Buf[Off+5]<<24);
            UINT32 ExtLen   = (UINT32)Buf[Off+10] | ((UINT32)Buf[Off+11]<<8) |
                              ((UINT32)Buf[Off+12]<<16)| ((UINT32)Buf[Off+13]<<24);
            BOOLEAN RecIsDir = (Flags & 0x02) != 0;

            if (IdLen == 1 && (Id[0] == 0x00 || Id[0] == 0x01)) { Off += RecLen; continue; }

            if (RecIsDir == IsDir && IsoNameEq(Id, IdLen, Name)) {
                *OutLba = ExtLba; *OutLen = ExtLen;
                Status = EFI_SUCCESS;
                break;
            }
        }
        Off += RecLen;
    }
    } /* ScanLen block */

    gBS->FreePool(Buf);
    return Status;
}

/* -----------------------------------------------------------------------
   Minimal in-memory FAT12/FAT16/FAT32 parser.
   Used to extract /EFI/BOOT/BOOTX64.EFI from a FAT partition image that
   Windows 10 embeds as its El Torito UEFI boot image.
   FatImg/FatSize: full FAT partition image already in memory.
   ----------------------------------------------------------------------- */

/* Return one byte from FAT image (returns 0 if out of range). */
static UINT8 FI(UINT8 *I, UINTN S, UINT64 O) { return (O < S) ? I[(UINTN)O] : 0u; }

/* Read a FAT16 chain entry for cluster C. Returns next cluster (0xFFFF = EOC). */
static UINT16 Fat16Next(UINT8 *Img, UINTN ImgSize, UINT32 FatByteOff, UINT32 C) {
    UINT32 O = FatByteOff + C * 2u;
    if (O + 1u >= ImgSize) return 0xFFFFu;
    return (UINT16)(Img[O] | ((UINT16)Img[O + 1u] << 8u));
}
/* Read a FAT12 chain entry for cluster C. */
static UINT16 Fat12Next(UINT8 *Img, UINTN ImgSize, UINT32 FatByteOff, UINT32 C) {
    UINT32 O = FatByteOff + (C * 3u) / 2u;
    UINT16 V;
    if (O + 1u >= ImgSize) return 0xFFFu;
    V = (UINT16)(Img[O] | ((UINT16)Img[O + 1u] << 8u));
    return (C & 1u) ? (V >> 4u) : (V & 0x0FFFu);
}
/* Read a FAT32 chain entry for cluster C. */
static UINT32 Fat32Next(UINT8 *Img, UINTN ImgSize, UINT32 FatByteOff, UINT32 C) {
    UINT32 O = FatByteOff + C * 4u;
    if (O + 3u >= ImgSize) return 0x0FFFFFFFu;
    return ((UINT32)Img[O] | ((UINT32)Img[O+1]<<8) | ((UINT32)Img[O+2]<<16) | ((UINT32)Img[O+3]<<24)) & 0x0FFFFFFFu;
}

/* Search directory entries (flat list) for an 8.3 name.
   DirOff: byte offset in image, DirBytes: max bytes to scan.
   Name: 8-char padded, Ext: 3-char padded (space-padded, uppercase).
   Returns first cluster and file size. */
static BOOLEAN FatFindEntry(UINT8 *Img, UINTN ImgSize,
                              UINT64 DirOff, UINT32 DirBytes,
                              const char *Name8, const char *Ext3,
                              BOOLEAN WantDir,
                              UINT32 *OutCluster, UINT32 *OutSize)
{
    UINT64 E;
    for (E = DirOff; E + 32u <= DirOff + DirBytes && E + 32u <= ImgSize; E += 32u) {
        UINT8 First = FI(Img, ImgSize, E);
        UINT8 Attr, n;
        BOOLEAN IsDir, Match;
        if (First == 0x00u) break;
        if (First == 0xE5u) continue;
        Attr  = FI(Img, ImgSize, E + 11u);
        if (Attr == 0x0Fu) continue; /* LFN */
        IsDir = (Attr & 0x10u) != 0u;
        if (IsDir != WantDir) continue;
        Match = 1;
        for (n = 0; n < 8u; n++) {
            UINT8 a = FI(Img, ImgSize, E + n);
            UINT8 b = (UINT8)Name8[n];
            if (a >= 'a' && a <= 'z') a -= 32u;
            if (b >= 'a' && b <= 'z') b -= 32u;
            if (a != b) { Match = 0; break; }
        }
        if (!Match) continue;
        for (n = 0; n < 3u; n++) {
            UINT8 a = FI(Img, ImgSize, E + 8u + n);
            UINT8 b = (UINT8)Ext3[n];
            if (a >= 'a' && a <= 'z') a -= 32u;
            if (b >= 'a' && b <= 'z') b -= 32u;
            if (a != b) { Match = 0; break; }
        }
        if (!Match) continue;
        {
            UINT16 ClHi = (UINT16)(FI(Img,ImgSize,E+20u) | ((UINT16)FI(Img,ImgSize,E+21u)<<8u));
            UINT16 ClLo = (UINT16)(FI(Img,ImgSize,E+26u) | ((UINT16)FI(Img,ImgSize,E+27u)<<8u));
            *OutCluster = ((UINT32)ClHi << 16u) | ClLo;
            *OutSize    = (UINT32)FI(Img,ImgSize,E+28u) | ((UINT32)FI(Img,ImgSize,E+29u)<<8u) |
                          ((UINT32)FI(Img,ImgSize,E+30u)<<16u) | ((UINT32)FI(Img,ImgSize,E+31u)<<24u);
            return 1;
        }
    }
    return 0;
}

/* Read up to ReadSize bytes of file starting at cluster C into OutBuf.
   Follows cluster chain (FAT12/16 or FAT32). Returns bytes read. */
static UINTN FatReadFile(UINT8 *Img, UINTN ImgSize,
                          UINT32 FatByteOff, UINT32 DataStart, UINT32 SPC, UINT32 BPS,
                          UINT8 FatType, /* 12, 16, or 32 */
                          UINT32 FirstCluster, UINT8 *OutBuf, UINTN ReadSize)
{
    UINT32 Cluster = FirstCluster;
    UINTN  Written = 0;
    UINTN  ClBytes = (UINTN)SPC * BPS;

    while (Written < ReadSize && Cluster >= 2u) {
        UINT64 ClOff = (UINT64)DataStart * BPS + (UINT64)(Cluster - 2u) * ClBytes;
        UINTN  Take  = ClBytes;
        UINTN  i;
        if (Written + Take > ReadSize) Take = ReadSize - Written;
        if (ClOff >= ImgSize) {
            return Written;
        }
        for (i = 0; i < Take && ClOff + i < ImgSize; i++)
            OutBuf[Written + i] = Img[(UINTN)(ClOff + i)];
        Written += i;
        if (i < Take) {
            return Written;
        }
        /* Next cluster */
        if (FatType == 12u) {
            UINT16 N = Fat12Next(Img, ImgSize, FatByteOff, Cluster);
            if (N >= 0xFF8u) break;
            Cluster = N;
        } else if (FatType == 16u) {
            UINT16 N = Fat16Next(Img, ImgSize, FatByteOff, Cluster);
            if (N >= 0xFFF8u) break;
            Cluster = N;
        } else {
            UINT32 N = Fat32Next(Img, ImgSize, FatByteOff, Cluster);
            if (N >= 0x0FFFFFF8u) break;
            Cluster = N;
        }
    }
    return Written;
}

/* Walk a FAT cluster chain to find a directory entry by 8.3 name.
   Handles directories that span multiple clusters (large root or sub-dir).
   Returns 1 if found and writes OutCluster/OutSize. */
static BOOLEAN FatFindInChain(UINT8 *Img, UINTN ImgSize,
                                UINT32 FatByteOff, UINT32 DataStart, UINT32 SPC, UINT32 BPS,
                                UINT8 FatType, UINT32 StartCluster,
                                const char *Name8, const char *Ext3, BOOLEAN WantDir,
                                UINT32 *OutCluster, UINT32 *OutSize)
{
    UINT32 Cluster = StartCluster;
    UINT32 Hops    = 0;
    UINTN  ClBytes = (UINTN)SPC * BPS;
    while (Cluster >= 2u && Hops < 65536u) {
        UINT64 Off = (UINT64)DataStart * BPS + (UINT64)(Cluster - 2u) * ClBytes;
        if (Off >= ImgSize) return 0;
        if (FatFindEntry(Img, ImgSize, Off, (UINT32)ClBytes,
                          Name8, Ext3, WantDir, OutCluster, OutSize))
            return 1;
        if (FatType == 12u) {
            UINT16 N = Fat12Next(Img, ImgSize, FatByteOff, Cluster);
            if (N >= 0xFF8u) return 0;
            Cluster = N;
        } else if (FatType == 16u) {
            UINT16 N = Fat16Next(Img, ImgSize, FatByteOff, Cluster);
            if (N >= 0xFFF8u) return 0;
            Cluster = N;
        } else {
            UINT32 N = Fat32Next(Img, ImgSize, FatByteOff, Cluster);
            if (N >= 0x0FFFFFF8u) return 0;
            Cluster = N;
        }
        Hops++;
    }
    return 0;
}

/* Try to find and boot /EFI/BOOT/BOOTX64.EFI from a FAT image in memory.
   Called when El Torito image is a FAT partition image (Windows 10 style). */
static EFI_STATUS BootFromFatImageBuf(UINT8 *Img, UINTN ImgSize) {
    UINT16 BPS, RootEntCnt, RSVD, FATSz16;
    UINT8  SPC, NumFAT, FatType;
    UINT32 FATSz, DataStart, FatByteOff;
    UINT32 RootDirSectors, RootDirOff;
    UINT32 EfiCluster, EfiSize, BootCluster, BootSize, FileCluster, FileSize;
    UINT8 *FileBuf;
    UINTN  FileRead;
    EFI_STATUS Status;
    const CHAR16 *ValidationError = 0;

    if (ImgSize < 512u) return IsoFail(EFI_NOT_FOUND, L"El Torito image is too small to contain a FAT ESP.");
    /* Boot sector signature 0x55AA at offset 510 — quick sanity check */
    if (Img[510] != 0x55u || Img[511] != 0xAAu) {
        /* Not a fatal failure — some FAT images skip this. Fall through. */
    }

    BPS        = (UINT16)(Img[0x0B] | ((UINT16)Img[0x0C] << 8u));
    SPC        = Img[0x0D];
    RSVD       = (UINT16)(Img[0x0E] | ((UINT16)Img[0x0F] << 8u));
    NumFAT     = Img[0x10];
    RootEntCnt = (UINT16)(Img[0x11] | ((UINT16)Img[0x12] << 8u));
    FATSz16    = (UINT16)(Img[0x16] | ((UINT16)Img[0x17] << 8u));

    if (BPS < 512u || BPS > 4096u || SPC == 0u || RSVD == 0u || NumFAT == 0u) {
        return IsoFail(EFI_NOT_FOUND, L"El Torito image is not a supported FAT ESP.");
    }
    /* SPC must be a power of two between 1 and 128 */
    if ((SPC & (SPC - 1u)) != 0u || SPC > 128u) {
        return IsoFail(EFI_NOT_FOUND, L"FAT ESP cluster size is invalid.");
    }

    {
        UINT32 FATSz32 = (UINT32)Img[0x24]|((UINT32)Img[0x25]<<8)|((UINT32)Img[0x26]<<16)|((UINT32)Img[0x27]<<24);
        FATSz = FATSz16 ? (UINT32)FATSz16 : FATSz32;
    }
    if (FATSz == 0u) return IsoFail(EFI_NOT_FOUND, L"FAT ESP has an invalid FAT size.");

    RootDirSectors = ((UINT32)RootEntCnt * 32u + BPS - 1u) / BPS;
    DataStart      = (UINT32)RSVD + (UINT32)NumFAT * FATSz + RootDirSectors;
    FatByteOff     = (UINT32)RSVD * BPS; /* FAT1 byte offset */

    /* Determine FAT type by cluster count */
    {
        UINT32 TotSec16 = (UINT32)(Img[0x13] | ((UINT16)Img[0x14] << 8u));
        UINT32 TotSec32 = (UINT32)Img[0x20]|((UINT32)Img[0x21]<<8)|((UINT32)Img[0x22]<<16)|((UINT32)Img[0x23]<<24);
        UINT32 TotSec   = TotSec16 ? TotSec16 : TotSec32;
        UINT32 DataSecs = (TotSec > DataStart) ? TotSec - DataStart : 0u;
        UINT32 ClCnt    = DataSecs / SPC;
        FatType = (ClCnt < 4085u) ? 12u : (ClCnt < 65525u) ? 16u : 32u;
    }

    /* Step 1: find "EFI     " directory in root */
    if (FatType == 32u) {
        UINT32 RootClus32 = (UINT32)Img[0x2C]|((UINT32)Img[0x2D]<<8)|((UINT32)Img[0x2E]<<16)|((UINT32)Img[0x2F]<<24);
        if (RootClus32 < 2u) return IsoFail(EFI_NOT_FOUND, L"FAT32 ESP root directory is invalid.");
        if (!FatFindInChain(Img, ImgSize, FatByteOff, DataStart, SPC, BPS, FatType,
                            RootClus32, "EFI     ", "   ", 1, &EfiCluster, &EfiSize))
            return IsoFail(EFI_NOT_FOUND, L"FAT ESP does not contain /EFI.");
        (void)RootDirOff;
    } else {
        RootDirOff = (UINT32)RSVD + (UINT32)NumFAT * FATSz;
        if (!FatFindEntry(Img, ImgSize, (UINT64)RootDirOff * BPS, RootDirSectors * BPS,
                          "EFI     ", "   ", 1, &EfiCluster, &EfiSize))
            return IsoFail(EFI_NOT_FOUND, L"FAT ESP does not contain /EFI.");
    }

    if (EfiCluster < 2u) return IsoFail(EFI_NOT_FOUND, L"/EFI directory cluster is invalid.");

    /* Step 2: find "BOOT    " directory in EFI (follow the chain) */
    if (!FatFindInChain(Img, ImgSize, FatByteOff, DataStart, SPC, BPS, FatType,
                        EfiCluster, "BOOT    ", "   ", 1, &BootCluster, &BootSize))
        return IsoFail(EFI_NOT_FOUND, L"FAT ESP does not contain /EFI/BOOT.");

    if (BootCluster < 2u) return IsoFail(EFI_NOT_FOUND, L"/EFI/BOOT directory cluster is invalid.");

    /* Step 3: find "BOOTX64 " "EFI" file in BOOT (follow the chain) */
    if (!FatFindInChain(Img, ImgSize, FatByteOff, DataStart, SPC, BPS, FatType,
                        BootCluster, "BOOTX64 ", "EFI", 0, &FileCluster, &FileSize))
        return IsoFail(EFI_NOT_FOUND, L"FAT ESP does not contain /EFI/BOOT/BOOTX64.EFI.");

    if (FileSize == 0u || FileSize > 32u * 1024u * 1024u) {
        return IsoFail(EFI_NOT_FOUND, L"BOOTX64.EFI size is invalid.");
    }
    if (FileCluster < 2u) return IsoFail(EFI_NOT_FOUND, L"BOOTX64.EFI cluster is invalid.");

    Status = gBS->AllocatePool(EFI_LOADER_DATA, FileSize, (void **)&FileBuf);
    if (EFI_ERROR(Status)) return Status;

    FileRead = FatReadFile(Img, ImgSize, FatByteOff, DataStart, SPC, BPS, FatType,
                           FileCluster, FileBuf, FileSize);
    if (FileRead < FileSize) {
        gBS->FreePool(FileBuf);
        return IsoFail(EFI_NOT_FOUND, L"BOOTX64.EFI could not be fully read from FAT ESP.");
    }

    Status = ValidateX64UefiImage(FileBuf, FileSize, &ValidationError);
    if (EFI_ERROR(Status)) {
        gBS->FreePool(FileBuf);
        return IsoFail(Status, ValidationError);
    }

    Status = LoadAndStartFromBuffer(FileBuf, FileSize);
    if (EFI_ERROR(Status)) {
        IsoFail(Status, L"UEFI image returned an error. Windows ISO needs virtual CD/DVD mapping.");
    }
    gBS->FreePool(FileBuf);
    return Status;
}

/* Parse El Torito catalog: return LBA + byte-size of a UEFI boot image.
   Priority: UEFI (0xEF) section entry > UEFI validation/default entry.
   BIOS no-emulation entries are intentionally rejected. Treating isolinux or
   other BIOS boot sectors as UEFI images creates false "bad UEFI binary" errors.
   Windows 10 ISO has: Validation Entry platform=0x00 (BIOS), Default Entry = BIOS,
   then Section Header platform=0xEF with a UEFI Section Entry. */
static EFI_STATUS ParseElTorito(ISO_READER *R, UINT32 CatLba,
                                  UINT32 *OutLba, UINT32 *OutBytes)
{
    EFI_STATUS Status;
    UINT8 *Cat;
    UINTN  Off;
    UINT8 ValidationPlatform;

    /* Read 4 sectors of catalog — enough for all Windows / Ubuntu layouts */
    Status = gBS->AllocatePool(EFI_LOADER_DATA, ISO_SECTOR_SIZE * 4, (void **)&Cat);
    if (EFI_ERROR(Status)) return Status;

    Status = IsoRead(R, CatLba, 4, Cat);
    if (EFI_ERROR(Status)) { gBS->FreePool(Cat); return Status; }

    /* Validation Entry (32 bytes): byte 0=0x01, bytes 30-31=0x55/0xAA */
    if (Cat[0] != 0x01 || Cat[30] != 0x55 || Cat[31] != 0xAA) {
        gBS->FreePool(Cat); return EFI_NOT_FOUND;
    }
    ValidationPlatform = Cat[1];

    /* Default/Initial Entry at offset 32. Accept only if the catalog itself is UEFI. */
    if (ValidationPlatform == ELTORITO_UEFI && Cat[32] == ELTORITO_BOOTABLE) {
        UINT8  Media  = Cat[33]; /* 0=no-emulation */
        UINT32 DLba   = (UINT32)Cat[40]|((UINT32)Cat[41]<<8)|((UINT32)Cat[42]<<16)|((UINT32)Cat[43]<<24);
        UINT16 DCnt   = (UINT16)(Cat[38]|(Cat[39]<<8));
        UINT32 DBytes = DCnt ? (UINT32)DCnt * 512u : 2u * 1024u * 1024u;
        if (Media == 0) {
            *OutLba = DLba;
            *OutBytes = DBytes;
            gBS->FreePool(Cat);
            return EFI_SUCCESS;
        }
    }

    /* Walk Section Headers starting at offset 64 */
    Off = 64;
    while (Off + 32 <= ISO_SECTOR_SIZE * 4) {
        UINT8  Ind = Cat[Off];
        UINT8  Plt = Cat[Off + 1];
        UINT16 Num = (UINT16)(Cat[Off + 2] | ((UINT16)Cat[Off + 3] << 8));
        UINT16 e;

        if (Ind != 0x90 && Ind != 0x91) break; /* not a section header */

        Off += 32; /* advance to first section entry */
        for (e = 0; e < Num && Off + 32 <= ISO_SECTOR_SIZE * 4; e++, Off += 32) {
            if (Cat[Off] == ELTORITO_BOOTABLE) {
                UINT32 Lba   = (UINT32)Cat[Off+8]|((UINT32)Cat[Off+9]<<8)|
                               ((UINT32)Cat[Off+10]<<16)|((UINT32)Cat[Off+11]<<24);
                UINT16 Cnt   = (UINT16)(Cat[Off+6]|(Cat[Off+7]<<8));
                UINT32 Bytes = Cnt ? (UINT32)Cnt * 512u : 2u * 1024u * 1024u;
                if (Plt == ELTORITO_UEFI) {
                    /* Best match: UEFI section entry */
                    *OutLba   = Lba;
                    *OutBytes = Bytes;
                    gBS->FreePool(Cat);
                    return EFI_SUCCESS;
                }
            }
        }
        if (Ind == 0x91) break; /* last section header */
    }

    gBS->FreePool(Cat);
    return EFI_NOT_FOUND;
}

static void AppendAsciiLiteral(char *Buffer, UINTN BufferSize, UINTN *Offset, const char *Text) {
    while (*Text != 0 && *Offset + 1u < BufferSize) {
        Buffer[*Offset] = *Text;
        *Offset = *Offset + 1u;
        Text++;
    }
    if (BufferSize > 0) Buffer[*Offset] = 0;
}

static void AppendAsciiFromChar16(char *Buffer, UINTN BufferSize, UINTN *Offset, const CHAR16 *Text) {
    UINTN Index;
    for (Index = 0; Text[Index] != 0 && *Offset + 1u < BufferSize; Index++) {
        CHAR16 Ch = Text[Index];
        Buffer[*Offset] = (Ch >= 0x20 && Ch < 0x7Fu) ? (char)Ch : '?';
        *Offset = *Offset + 1u;
    }
    if (BufferSize > 0) Buffer[*Offset] = 0;
}

static EFI_STATUS WriteSelectedIsoHintToRoot(EFI_FILE_PROTOCOL *Root, BOOT_ENTRY *Entry) {
    EFI_FILE_PROTOCOL *File = 0;
    EFI_STATUS Status;
    char Buffer[768];
    UINTN Offset = 0;
    UINTN Size;

    if (Root == 0 || Entry == 0 || Root->Open == 0) return EFI_INVALID_PARAMETER;

    AppendAsciiLiteral(Buffer, sizeof(Buffer), &Offset, "path=");
    AppendAsciiFromChar16(Buffer, sizeof(Buffer), &Offset, Entry->Path);
    AppendAsciiLiteral(Buffer, sizeof(Buffer), &Offset, "\r\nname=");
    AppendAsciiFromChar16(Buffer, sizeof(Buffer), &Offset, Entry->Name);
    AppendAsciiLiteral(Buffer, sizeof(Buffer), &Offset, "\r\n");

    Status = Root->Open(Root, &File,
                        L"\\CaYaISOBoot\\cache\\selected-windows-iso.txt",
                        EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE,
                        0);
    if (EFI_ERROR(Status) || File == 0 || File->Write == 0) return Status;

    if (File->SetPosition != 0) {
        (void)File->SetPosition(File, 0);
    }

    Size = Offset;
    Status = File->Write(File, &Size, Buffer);
    File->Close(File);
    return Status;
}

static void PersistSelectedIsoHint(BOOT_ENTRY *Entry) {
    if (Entry == 0 || Entry->Type != BootFileIso) return;

    /* UEFI Boot Services disappear after boot.wim starts.  Record the chosen
       ISO on a writable FAT volume so the WinPE bridge can mount it again. */
    (void)WriteSelectedIsoHintToRoot(gRoot, Entry);
    if (Entry->Root != 0 && Entry->Root != gRoot) {
        (void)WriteSelectedIsoHintToRoot(Entry->Root, Entry);
    }
}

static void HandleIsoOrImgAscii(BOOT_ENTRY *Entry) {
    EFI_STATUS         Status;
    ISO_READER         R;
    EFI_FILE_PROTOCOL *IsoFile  = 0;
    UINT8             *Sector   = 0;
    UINT32             CatLba   = 0;
    UINT32             ImgLba   = 0, ImgBytes = 0;
    void              *ImgBuf   = 0;
    UINT32             ImgSectors;
    BOOLEAN            VirtualIsoInstalled = 0;
    EFI_HANDLE         VirtualCdHandle     = 0;

    Zero(&R, sizeof(R));
    gIsoFailureDetail = 0;
    Clear();
    DrawGraphicChrome(0, 0);
    HideCursor();
    DrawTopText(L"ISO/IMG handler");
    SetTextAttribute(EFI_LIGHTGRAY, EFI_BLACK);
    PrintAt(3, 6,  L"Selected file: "); PrintLimited(Entry->Name, 64);
    PrintAt(3, 8,  L"Path: ");          PrintLimited(Entry->Path, 70);
    PrintAt(3, 10, L"Type: ");          Print((CHAR16 *)TypeName(Entry->Type));

    if (Entry->Type == BootFileImg) {
        SetTextAttribute(EFI_YELLOW, EFI_BLACK);
        PrintAt(3, 13, L"Raw IMG boot is not yet implemented.");
        SetTextAttribute(EFI_LIGHTGRAY, EFI_BLACK);
        PrintAt(3, 17, L"Press any key to return.");
        WaitKey();
        return;
    }

    SetTextAttribute(EFI_LIGHTGRAY, EFI_BLACK);
    PrintAt(3, 12, L"Opening ISO file...                      ");
    PersistSelectedIsoHint(Entry);

    if (Entry->ExVol != 0) {
        /* exFAT Block I/O path — no file handle needed */
        R.IsExfat      = 1;
        R.ExVol        = Entry->ExVol;
        R.ExCluster    = Entry->ExCluster;
        R.ExNoFatChain = Entry->ExNoFatChain;
    } else {
        /* FAT SimpleFileSystem path */
        Status = Entry->Root->Open(Entry->Root, &IsoFile, Entry->Path, EFI_FILE_MODE_READ, 0);
        if (EFI_ERROR(Status)) {
            SetTextAttribute(EFI_LIGHTRED, EFI_BLACK);
            PrintAt(3, 14, L"Could not open ISO file.");
            SetTextAttribute(EFI_LIGHTGRAY, EFI_BLACK);
            PrintAt(3, 17, L"Press any key to return.");
            WaitKey();
            return;
        }
        R.IsExfat = 0;
        R.File    = IsoFile;
    }

    Status = gBS->AllocatePool(EFI_LOADER_DATA, ISO_SECTOR_SIZE * 2, (void **)&Sector);
    if (EFI_ERROR(Status)) goto OomError;

    /* Read and verify PVD at ISO LBA 16 = byte offset 32768 */
    PrintAt(3, 13, L"Reading ISO 9660 PVD...                  ");
    Status = IsoRead(&R, ISO_PVD_LBA, 1, Sector);
    if (EFI_ERROR(Status)) goto ReadError;

    if (Sector[0] != 1 ||
        Sector[1] != 'C' || Sector[2] != 'D' ||
        Sector[3] != '0' || Sector[4] != '0' || Sector[5] != '1') {
        SetTextAttribute(EFI_LIGHTRED, EFI_BLACK);
        PrintAt(3, 14, L"Invalid ISO 9660 signature - file may not be a valid ISO.");
        SetTextAttribute(EFI_LIGHTGRAY, EFI_BLACK);
        gBS->FreePool(Sector); Sector = 0;
        if (IsoFile) { IsoFile->Close(IsoFile); IsoFile = 0; }
        PrintAt(3, 17, L"Press any key to return.");
        WaitKey();
        return;
    }
    PrintAt(3, 13, L"ISO 9660 PVD verified.                   ");

    /* Root directory entry is at PVD offset 156 (34-byte record) */
    {
        UINT32 RootLba = (UINT32)Sector[158]|((UINT32)Sector[159]<<8)|
                         ((UINT32)Sector[160]<<16)|((UINT32)Sector[161]<<24);
        UINT32 RootLen = (UINT32)Sector[166]|((UINT32)Sector[167]<<8)|
                         ((UINT32)Sector[168]<<16)|((UINT32)Sector[169]<<24);

        /* Scan VDs at LBA 17+ for the El Torito Boot Record VD */
        {
            UINT32 VdLba;
            for (VdLba = ISO_PVD_LBA + 1; VdLba < ISO_PVD_LBA + 32; VdLba++) {
                Status = IsoRead(&R, VdLba, 1, Sector);
                if (EFI_ERROR(Status)) break;
                if (Sector[0] == 255) break; /* Volume Descriptor Set Terminator */
                if (Sector[0] == 0 &&
                    Sector[7]=='E' && Sector[8]=='L' && Sector[9]==' ' &&
                    Sector[10]=='T' && Sector[11]=='O' && Sector[12]=='R' &&
                    Sector[13]=='I' && Sector[14]=='T' && Sector[15]=='O') {
                    CatLba = (UINT32)Sector[71]|((UINT32)Sector[72]<<8)|
                             ((UINT32)Sector[73]<<16)|((UINT32)Sector[74]<<24);
                    break;
                }
            }
        }

        gBS->FreePool(Sector); Sector = 0;

        /* Strategy 1: ISO directory tree scan for /EFI/BOOT/BOOTX64.EFI
           This is the most reliable path for modern UEFI-bootable ISOs (Windows, Ubuntu, Fedora).
           El Torito may point to a raw FAT image that LoadImage cannot handle directly. */
        {
            UINT32 EfiDirLba = 0, EfiDirLen = 0;
            UINT32 BootDirLba = 0, BootDirLen = 0;
            UINT32 FileLba = 0, FileLen = 0;

            PrintAt(3, 14, L"Scanning /EFI/BOOT/BOOTX64.EFI...       ");

            Status = IsoFindEntry(&R, RootLba, RootLen, "EFI", 1, &EfiDirLba, &EfiDirLen);
            if (EFI_ERROR(Status)) {
                /* try with larger root len in case PVD len is truncated */
                if (RootLen < 2048u) RootLen = 2048u;
                Status = IsoFindEntry(&R, RootLba, RootLen, "EFI", 1, &EfiDirLba, &EfiDirLen);
            }
            if (!EFI_ERROR(Status)) {
                Status = IsoFindEntry(&R, EfiDirLba, EfiDirLen, "BOOT", 1, &BootDirLba, &BootDirLen);
            }
            if (!EFI_ERROR(Status)) {
                Status = IsoFindEntry(&R, BootDirLba, BootDirLen, "BOOTX64.EFI", 0, &FileLba, &FileLen);
            }

            if (!EFI_ERROR(Status) && FileLba != 0 && FileLen != 0) {
                /* Ventoy-style preferred path: expose the whole ISO as a
                   virtual CD-ROM and let firmware's ISO9660 driver mount it,
                   then chainload \EFI\BOOT\BOOTX64.EFI through that filesystem.
                   This makes BOOTX64.EFI's loaded image device handle the
                   virtual CD, so when the Windows boot manager looks for
                   \sources\boot.wim or the Windows installer looks for setup
                   media, it finds them on the same handle. */
                if (!VirtualIsoInstalled) {
                    EFI_STATUS CdStatus;
                    PrintAt(3, 15, L"Installing virtual ISO CD/DVD...        ");
                    CdStatus = InstallIsoVirtualCd(&R, Entry->Size, &VirtualCdHandle);
                    if (!EFI_ERROR(CdStatus)) {
                        VirtualIsoInstalled = 1;
                        if (!R.IsExfat) {
                            IsoFile = 0; /* keep file handle alive for virtual CD */
                        }
                    }
                }
                if (VirtualIsoInstalled && VirtualCdHandle != 0) {
                    EFI_STATUS VcdStatus;
                    PrintAt(3, 16, L"Booting BOOTX64.EFI via virtual CD...   ");
                    VcdStatus = BootBootx64FromHandle(VirtualCdHandle);
                    if (!EFI_ERROR(VcdStatus)) return;
                }

                /* Fallback: load BOOTX64.EFI from a memory buffer. Works for
                   most non-Windows ISOs, but Windows installer usually fails
                   here with "no media driver" because boot.wim is not on the
                   same loaded image device. */
                ImgSectors = (FileLen + ISO_SECTOR_SIZE - 1) / ISO_SECTOR_SIZE;
                Status = gBS->AllocatePool(EFI_LOADER_DATA, (UINTN)ImgSectors * ISO_SECTOR_SIZE, &ImgBuf);
                if (EFI_ERROR(Status)) goto OomError;
                PrintAt(3, 15, L"Loading /EFI/BOOT/BOOTX64.EFI...        ");
                Status = IsoRead(&R, FileLba, ImgSectors, ImgBuf);
                if (!EFI_ERROR(Status)) {
                    if (IsoFile && !VirtualIsoInstalled) { IsoFile->Close(IsoFile); IsoFile = 0; }
                    PrintAt(3, 16, L"Launching BOOTX64.EFI (buffer)...       ");
                    Status = LoadAndStartFromBuffer(ImgBuf, (UINTN)FileLen);
                }
                gBS->FreePool(ImgBuf); ImgBuf = 0;
                if (!EFI_ERROR(Status)) return;
                SetTextAttribute(EFI_YELLOW, EFI_BLACK);
                PrintAt(3, 17, L"BOOTX64.EFI returned, trying El Torito...");
                SetTextAttribute(EFI_LIGHTGRAY, EFI_BLACK);
                /* IsoFile may be closed — re-open if needed for El Torito */
                if (IsoFile == 0 && !R.IsExfat && !VirtualIsoInstalled) {
                    Entry->Root->Open(Entry->Root, &IsoFile, Entry->Path, EFI_FILE_MODE_READ, 0);
                    R.File = IsoFile;
                }
            } else {
                PrintAt(3, 15, L"EFI/BOOT/BOOTX64.EFI not in dir tree.   ");
            }
        }

        /* Strategy 2: El Torito — UEFI boot image is almost always a FAT
           partition (Windows / Ubuntu / TinyCore). El Torito Section Entry
           sector count is unreliable (often Cnt=1) so we always probe the
           FAT BPB first to compute the real image size before reading. */
        if (CatLba != 0) {
            UINT8  *Probe = 0;
            UINT32  TrueImgBytes = 0;
            UINT32  FatBlockSize = 512;
            BOOLEAN LooksLikeFat = 0;
            BOOLEAN LooksLikePe  = 0;

            PrintAt(3, 16, L"Trying El Torito catalog...              ");
            Status = ParseElTorito(&R, CatLba, &ImgLba, &ImgBytes);
            if (!EFI_ERROR(Status)) {
                Status = gBS->AllocatePool(EFI_LOADER_DATA, ISO_SECTOR_SIZE, (void **)&Probe);
                if (EFI_ERROR(Status)) goto OomError;
                Status = IsoRead(&R, ImgLba, 1, Probe);
                if (!EFI_ERROR(Status)) {
                    UINT16 PrBPS = (UINT16)(Probe[0x0B] | ((UINT16)Probe[0x0C] << 8u));
                    UINT8  PrSPC = Probe[0x0D];
                    UINT16 PrRSV = (UINT16)(Probe[0x0E] | ((UINT16)Probe[0x0F] << 8u));
                    UINT8  PrNFA = Probe[0x10];
                    UINT32 PrTS16= (UINT32)(Probe[0x13] | ((UINT16)Probe[0x14] << 8u));
                    UINT32 PrTS32= (UINT32)Probe[0x20]|((UINT32)Probe[0x21]<<8)|
                                   ((UINT32)Probe[0x22]<<16)|((UINT32)Probe[0x23]<<24);
                    UINT32 PrTS  = PrTS16 ? PrTS16 : PrTS32;
                    /* FAT BPB sanity: BPS power-of-two 512..4096, SPC nonzero p2,
                       reserved sectors >= 1, FAT count 1 or 2, total sectors > 0. */
                    if (PrBPS >= 512u && PrBPS <= 4096u && (PrBPS & (PrBPS - 1u)) == 0u &&
                        PrSPC > 0u && (PrSPC & (PrSPC - 1u)) == 0u && PrSPC <= 128u &&
                        PrRSV >= 1u && (PrNFA == 1u || PrNFA == 2u) && PrTS > 0u) {
                        UINT64 FatSize = (UINT64)PrTS * PrBPS;
                        if (FatSize >= 64u * 1024u && FatSize <= 256ull * 1024ull * 1024ull) {
                            TrueImgBytes  = (UINT32)FatSize;
                            FatBlockSize  = PrBPS;
                            LooksLikeFat  = 1;
                        }
                    }
                    if (Probe[0] == 'M' && Probe[1] == 'Z') {
                        LooksLikePe = 1;
                    }
                }
                gBS->FreePool(Probe); Probe = 0;

                /* Use BPB-derived size when present; otherwise trust El Torito Cnt. */
                if (TrueImgBytes > ImgBytes) ImgBytes = TrueImgBytes;
                if (ImgBytes < 64u * 1024u) ImgBytes = 2u * 1024u * 1024u; /* sane floor */

                ImgSectors = (ImgBytes + ISO_SECTOR_SIZE - 1) / ISO_SECTOR_SIZE;
                Status = gBS->AllocatePool(EFI_LOADER_DATA, (UINTN)ImgSectors * ISO_SECTOR_SIZE, &ImgBuf);
                if (EFI_ERROR(Status)) goto OomError;
                PrintAt(3, 17, L"Loading El Torito image...               ");
                Status = IsoRead(&R, ImgLba, ImgSectors, ImgBuf);
                if (!EFI_ERROR(Status)) {
                    if (LooksLikeFat) {
                        /* It's a FAT partition image. Install the virtual ISO
                           CD first; this is what makes Windows installer find
                           \sources\boot.wim after BOOTX64.EFI takes over. */
                        if (!VirtualIsoInstalled) {
                            EFI_STATUS CdStatus;
                            PrintAt(3, 18, L"Installing virtual ISO CD/DVD...         ");
                            CdStatus = InstallIsoVirtualCd(&R, Entry->Size, &VirtualCdHandle);
                            if (!EFI_ERROR(CdStatus)) {
                                VirtualIsoInstalled = 1;
                                if (!R.IsExfat) {
                                    IsoFile = 0; /* Keep file handle alive for virtual CD reads. */
                                }
                            }
                        }

                        /* Ventoy approach: try to chainload BOOTX64.EFI through
                           the virtual ISO CD's filesystem. BOOTX64.EFI then
                           sees the whole ISO content under its loaded device. */
                        if (VirtualIsoInstalled && VirtualCdHandle != 0) {
                            EFI_STATUS VcdStatus;
                            PrintAt(3, 18, L"Booting BOOTX64.EFI via virtual CD...    ");
                            VcdStatus = BootBootx64FromHandle(VirtualCdHandle);
                            if (!EFI_ERROR(VcdStatus)) {
                                gBS->FreePool(ImgBuf); ImgBuf = 0;
                                return;
                            }
                        }

                        PrintAt(3, 18, L"Mounting UEFI El Torito FAT image...     ");
                        Status = BootFromFatImageDevice((UINT8 *)ImgBuf,
                                                        (UINTN)ImgSectors * ISO_SECTOR_SIZE,
                                                        FatBlockSize,
                                                        ImgLba,
                                                        ImgSectors,
                                                        1);
                        if (EFI_ERROR(Status)) {
                            const CHAR16 *DeviceFailure = gIsoFailureDetail;
                            PrintAt(3, 18, L"Parsing FAT image for BOOTX64.EFI...     ");
                            Status = BootFromFatImageBuf((UINT8 *)ImgBuf, (UINTN)ImgSectors * ISO_SECTOR_SIZE);
                            if (EFI_ERROR(Status) && DeviceFailure != 0) {
                                gIsoFailureDetail = DeviceFailure;
                            }
                        }
                    } else if (LooksLikePe) {
                        /* It's a PE binary — load it directly. */
                        PrintAt(3, 18, L"Launching UEFI El Torito image...        ");
                        Status = LoadAndStartFromBuffer(ImgBuf, (UINTN)ImgBytes);
                    } else {
                        /* Unknown layout: try PE silently, then FAT fallback. */
                        Status = LoadAndStartFromBufferEx(ImgBuf, (UINTN)ImgBytes, 0);
                        if (EFI_ERROR(Status)) {
                            PrintAt(3, 18, L"Parsing FAT image for BOOTX64.EFI...     ");
                            Status = BootFromFatImageBuf((UINT8 *)ImgBuf, (UINTN)ImgSectors * ISO_SECTOR_SIZE);
                        }
                    }
                } else {
                    gIsoFailureDetail = L"Could not read the UEFI El Torito image from ISO.";
                }
                gBS->FreePool(ImgBuf); ImgBuf = 0;
                if (!EFI_ERROR(Status)) {
                    if (IsoFile && !VirtualIsoInstalled) { IsoFile->Close(IsoFile); IsoFile = 0; }
                    return;
                }
            } else {
                gIsoFailureDetail =
                    L"BIOS-only ISO (e.g. TinyCore, old Linux). Boot the USB in Legacy/CSM mode, "
                    L"or use a Linux distribution that ships /EFI/BOOT/BOOTX64.EFI.";
                SetTextAttribute(EFI_YELLOW, EFI_BLACK);
                PrintAt(3, 17, L"No UEFI El Torito entry was found.       ");
                SetTextAttribute(EFI_LIGHTGRAY, EFI_BLACK);
            }
        } else {
            gIsoFailureDetail =
                L"ISO has no El Torito boot catalog. This image is not bootable on UEFI firmware.";
            SetTextAttribute(EFI_YELLOW, EFI_BLACK);
            PrintAt(3, 16, L"No El Torito boot catalog was found.     ");
            SetTextAttribute(EFI_LIGHTGRAY, EFI_BLACK);
        }

        /* Both strategies failed */
        if (IsoFile && !VirtualIsoInstalled) { IsoFile->Close(IsoFile); IsoFile = 0; }
        SetTextAttribute(EFI_LIGHTRED, EFI_BLACK);
        PrintAt(3, 19, gIsoFailureDetail != 0 ? gIsoFailureDetail : L"ISO is not UEFI bootable or boot failed.");
        SetTextAttribute(EFI_LIGHTGRAY, EFI_BLACK);
        PrintAt(3, 21, L"Press any key to return.");
        WaitKey();
        return;
    }

ReadError:
    SetTextAttribute(EFI_LIGHTRED, EFI_BLACK);
    PrintAt(3, 15, L"ISO file read error.");
    SetTextAttribute(EFI_LIGHTGRAY, EFI_BLACK);
    if (Sector)  gBS->FreePool(Sector);
    if (IsoFile && !VirtualIsoInstalled) IsoFile->Close(IsoFile);
    PrintAt(3, 17, L"Press any key to return.");
    WaitKey();
    return;

OomError:
    SetTextAttribute(EFI_LIGHTRED, EFI_BLACK);
    PrintAt(3, 15, L"Out of memory.");
    SetTextAttribute(EFI_LIGHTGRAY, EFI_BLACK);
    if (Sector)  gBS->FreePool(Sector);
    if (ImgBuf)  gBS->FreePool(ImgBuf);
    if (IsoFile && !VirtualIsoInstalled) IsoFile->Close(IsoFile);
    PrintAt(3, 17, L"Press any key to return.");
    WaitKey();
    return;
}

extern "C" EFI_STATUS EfiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    EFI_STATUS Status;
    EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFs;
    BOOT_LIST List;
    UINTN Selected = 0;

    gImageHandle = ImageHandle;
    gST = SystemTable;
    gBS = SystemTable->BootServices;
    InitializeGraphics();
    HideCursor();

#if 0
    Clear();
    Header();
    Print(L"USB dosya sistemi açılıyor...\r\n");

#endif
    DrawStartupScreen();

    Status = gBS->HandleProtocol(ImageHandle, &LoadedImageProtocolGuid, (void **)&LoadedImage);
    if (EFI_ERROR(Status)) {
        Print(L"Loaded Image Protocol açılamadı.\r\n");
        WaitKey();
        return Status;
    }

    gDeviceHandle = LoadedImage->DeviceHandle;
    Status = gBS->HandleProtocol(gDeviceHandle, &SimpleFileSystemProtocolGuid, (void **)&SimpleFs);
    if (EFI_ERROR(Status)) {
        Print(L"Simple File System Protocol açılamadı.\r\n");
        WaitKey();
        return Status;
    }

    Status = SimpleFs->OpenVolume(SimpleFs, &gRoot);
    if (EFI_ERROR(Status)) {
        Print(L"USB kök dizini açılamadı.\r\n");
        WaitKey();
        return Status;
    }

    Zero(&List, sizeof(BOOT_LIST));
    TryLoadUefiDriver(gRoot, L"\\CaYaISOBoot\\drivers\\exfat.efi");
    ScanFiles(&List);
    for (;;) {
        EFI_INPUT_KEY Key;
        UINTN EventIndex;

        if (List.Count > 0 && Selected >= List.Count) {
            Selected = List.Count - 1;
        }

        DrawMenuAscii(&List, Selected);
        gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &EventIndex);
        if (gST->ConIn->ReadKeyStroke(gST->ConIn, &Key) == EFI_NOT_READY) {
            continue;
        }

        if (Key.ScanCode == SCAN_ESC) {
            break;
        }
        if (Key.ScanCode == SCAN_UP && List.Count > 0) {
            Selected = Selected == 0 ? List.Count - 1 : Selected - 1;
            continue;
        }
        if (Key.ScanCode == SCAN_DOWN && List.Count > 0) {
            Selected = (Selected + 1) % List.Count;
            continue;
        }
        if (Key.UnicodeChar == L'r' || Key.UnicodeChar == L'R') {
            ScanFiles(&List);
            Selected = 0;
            continue;
        }
        if ((Key.UnicodeChar == L'i' || Key.UnicodeChar == L'I') && List.Count > 0) {
            ShowEntryInfoAscii(&List.Entries[Selected]);
            continue;
        }
        if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN && List.Count > 0) {
            if (List.Entries[Selected].Type == BootFileEfi) {
                Status = StartEfiAscii(&List.Entries[Selected]);
                SetTextAttribute(EFI_LIGHTGRAY, EFI_BLACK);
                PrintAt(3, 18, L"EFI chainload returned. Press any key to continue.");
                WaitKey();
                continue;
#if 0
                Print(L"\r\nDevam etmek için bir tuşa basın.\r\n");
                WaitKey();
#endif
            } else {
                HandleIsoOrImgAscii(&List.Entries[Selected]);
            }
        }
    }

    CloseListRoots(&List);
    gRoot->Close(gRoot);
    return EFI_SUCCESS;
}
