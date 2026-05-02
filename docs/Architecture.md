# CaYaISOBoot Teknik Mimarisi

## A) Gerçekçi teknik mimari

CaYaISOBoot iki bağımsız ama birlikte çalışan parçadan oluşur:

1. Windows installer, USB bellekte klasör yapısını ve EFI payload dosyasını
   hazırlar. Varsayılan kurulum modu veri silmez.
2. CaYa Native Boot Engine, UEFI firmware tarafından
   `/EFI/BOOT/BOOTX64.EFI` olarak çalıştırılır, USB dosya sistemini açar,
   boot edilebilir dosyaları dinamik listeler ve kullanıcı seçimini yönetir.

Installer safe install modunda sadece dosya kopyalar ve klasör oluşturur.
`USB'yi Yapılandır` modu seçilen USB'yi iki aşamalı onaydan sonra iki bölüme
ayırabilir.

## B) Hazır bootloader kullanmadan UEFI app yaklaşımı

UEFI firmware zaten FAT tabanlı EFI System Partition üzerinden PE/COFF EFI
uygulaması çalıştırabilir. Bu nedenle MVP için en temiz yöntem kendi
`BOOTX64.EFI` uygulamamızı yazmaktır. EDK2 burada bootloader değil,
derleme ve UEFI header/library altyapısıdır. Menü, dosya tarama, EFI
chainload ve ISO handler kararları CaYa kodunda kalır.

## C) CaYa Native Boot Engine çalışma mantığı

Boot engine:

1. `Loaded Image Protocol` ile kendi yüklendiği device handle'ı bulur.
2. Aynı handle üzerinden `Simple File System Protocol` açar.
3. Kök dizini ve hedef ISO klasörlerini tarar.
4. `.iso`, `.img`, `.efi` uzantılı girdileri bellekte `BOOT_ENTRY` listesine alır.
5. Text UI menü çizer, klavye ile seçim yaptırır.
6. `.efi` için `LoadImage` / `StartImage` dener.
7. `.iso` için ISO9660/El Torito zincirini dener; Windows ISO'ları için
   WinPE bridge akışını hazırlar. `.img` için destek sınırını açıkça söyler.

GOP bulunan firmware'lerde menü başlığı grafik canvasa çizilir ve gömülü
CaYaDev logosu kullanılır. GOP bulunamazsa aynı menü Simple Text Output
üzerinden text-only olarak açılır.

## D) Ventoy benzeri dinamik kullanım mimarisi

USB bir kez hazırlanır. Clean install modunda CaYa sistem dosyaları CAYABOOT
bölümünde kalır; CAYAISO kullanıcıya boş veri bölümü olarak bırakılır.
Kullanıcı daha sonra ISO dosyalarını Windows uygulamasına dönmeden CAYAISO
köküne veya kendi oluşturduğu `/ISO/` klasörüne kopyalar. Safe install ve eski
kurulum uyumluluğu için `/CaYaISOBoot/ISO/` da taranmaya devam eder.
Boot engine listeyi her açılışta USB dosya sisteminden tekrar oluşturur.
Hazır config dosyası zorunlu değildir. İleri sürümlerde prepared config modu
uyumluluğu artırmak için ISO analizi ve cache bilgisi üretebilir.

## E) Windows Installer mimarisi

Installer katmanları:

- WPF UI: disk listesi, detay paneli, log paneli ve komutlar.
- ViewModel: async komutlar, seçili disk, progress ve onay akışı.
- Disk service: USB diskleri tarar, sistem diskini ve dahili diskleri eler.
- Install service: safe install klasörlerini oluşturur, EFI payload kopyalar.
- Logging service: UI ve dosya loglarını tek kanaldan yazar.

## F) Güvenli disk algılama sistemi

Algılama kuralları:

- `C:` ve Windows kök sürücüsü her durumda engellenir.
- Hazır olmayan, dosya sistemi okunamayan veya sistem volume olan sürücüler elenir.
- Win32 `DeviceIoControl(IOCTL_STORAGE_QUERY_PROPERTY)` ile bus tipi ve
  removable media bilgisi okunur.
- Dahili SATA/NVMe/SCSI diskler listelenmez.
- USB bağlantılı büyük diskler listelenebilir, fakat ekstra uyarıyla
  işaretlenir.
- Güvenlik denetimi başarısızsa kurulum komutu çalışmaz.

## G) USB'ye güvenli kurulum akışı

Safe install:

1. Disk seçilir.
2. İlk onay: "CaYaISOBoot bu USB belleğe kurulacak. Mevcut dosyalar korunacak."
3. İkinci onay: disk adı, harf, kapasite, dosya sistemi ve kurulum modu.
4. Klasörler oluşturulur.
5. `installer_payload/EFI/BOOT/BOOTX64.EFI` USB'ye kopyalanır.
6. Kurulum özeti ve log yazılır.

Yazılı doğrulama sadece format/temiz kurulum modunda kullanılır.

## H) USB klasör yapısı

Clean install iki bölümlüdür:

```text
CAYABOOT (FAT32, gizlenir)
  /EFI/BOOT/BOOTX64.EFI
  /Autounattend.xml
  /CaYaISOBoot/system/
  /CaYaISOBoot/config/
  /CaYaISOBoot/logs/
  /CaYaISOBoot/cache/
  /CaYaISOBoot/drivers/
  /CaYaISOBoot/drivers/winpe/
  /CaYaISOBoot/winpe/CaYaWinPeBridge.exe
  /$WinPEDriver$/

CAYAISO (kullanıcıya açık veri bölümü)
  <boş; kullanıcı ISO/IMG/EFI dosyalarını buraya veya /ISO altına koyar>
```

Safe install tek bölümlü olduğu için aynı dosyalar seçilen USB kökünde
oluşturulur.

## I) Dinamik ISO tarama sistemi

Boot engine yalnızca kontrollü yolları tarar:

- `/`
- `/ISO/`
- `/CaYaISOBoot/ISO/`

Varsayilan tarama kontrollu dizinlerde tutulur. Windows ISO secimi ayrica
cache dosyasina yazilir ki WinPE bridge hangi ISO'nun tekrar takilacagini
bilsin.

## J) ISO Boot Handler mimarisi

Handler sozlesmesi:

- Dosya destekleniyor mu?
- Hangi boot yontemi gerekir?
- Gerekli dosyalar analiz edilebildi mi?
- Baslatma mumkun degilse net sebep nedir?

Guncel handler'lar:

- `EfiFileHandler`: dogrudan chainload.
- `RawImageHandler`: algilar, boot etmez.
- `VentoyStyleIsoHandler`: runtime ISO akisini ve Windows WinPE bridge yolunu
  temsil eder.

Planlanan handler'lar:

- `GenericElToritoHandler`
- `LinuxIsoHandler`

## K) EFI dosyalarini chainload etme mantigi

Secili `.efi` dosyasi ayni USB device handle'i uzerinde bir file device path'e
cevrilir. Firmware `LoadImage(FALSE, ParentImage, DevicePath, NULL, 0, ...)`
ile child image'i yukler ve `StartImage` ile calistirir. Hata kodu kullaniciya
gosterilir; basarisiz chainload gizlenmez.

## L) C# .NET 8 WPF Installer MVP kodlari

Kodlar `src/` altinda dosya dosya ayrilmistir. UI MVVM tabanlidir, disk tarama
ve kurulum async calisir, nullable aciktir ve dis kaynak paket gerektirmez.

## M) C/C++ BOOTX64.EFI MVP kodlari

Kodlar `boot/CaYaNativeBootEngine/` altindadir. EDK2 ile derlenmek uzere
`CaYaNativeBootEngine.inf` eklenmistir. Kod EDK2 library API'lerini kullanir;
GRUB/Syslinux/Ventoy gibi bootloader kodu icermez.

## N) Build talimatlari

Detaylar `docs/Build.md` icindedir.

## O) Test senaryolari

Detaylar `docs/Build.md` ve `docs/ISOCompatibility.md` icindedir.

## P) Guvenlik onlemleri

Detaylar `docs/Security.md` icindedir.

## Q) Sinirlamalar ve uyumluluk

Windows ISO'lari icin WinPE bridge gerekir; bu bridge Virtual Disk API ile
secilen ISO'yu Windows Setup icinden tekrar takar. Secure Boot icin imzali EFI
binary gerekir. Legacy BIOS desteklenmez.

## R) Sonraki surum yol haritasi

Detaylar `docs/Roadmap.md` icindedir.
