# CaYaISOBoot

**Native Multi-ISO USB Boot System** — UEFI ortamında çalışan özgün çok-ISO USB boot yöneticisi.

---

## Nedir?

CaYaISOBoot, tek bir USB bellekten birden fazla `.iso`, `.img` ve `.efi` dosyasını
UEFI firmware üzerinde doğrudan başlatabilen, iki bileşenli bir sistemdir.

- **Windows tarafı:** .NET 8 WPF tabanlı installer uygulaması USB'yi hazırlar,
  önyükleme payload'ını kopyalar ve gerektiğinde diski bölümlendirir.
- **UEFI tarafı:** EDK2 ile derlenen `CaYaNativeBootEngine`, bilgisayar
  başladığında firmware tarafından `BOOTX64.EFI` olarak çalıştırılır, USB dosya
  sistemini tarar ve kullanıcıya bir boot menüsü sunar.

Üçüncü taraf bootloader kullanılmaz. Her iki bileşen de tamamen özgün koddur.

---

## Özellikler

- USB üzerindeki `.iso`, `.img` ve `.efi` dosyalarını boot anında dinamik olarak listeler; önceden bir config dosyası oluşturmak gerekmez.
- `.efi` dosyalarını UEFI `LoadImage` / `StartImage` ile doğrudan zincirler.
- Windows kurulum ISO'ları için WinPE aşamasında ISO'yu Windows Virtual Disk API'siyle read-only olarak yeniden takar (`CaYaWinPeBridge`).
- UEFI GOP bulunan cihazlarda grafik başlık ve gömülü CaYaDev logosu; GOP yoksa otomatik text-only mod.
- `USB'yi Yapılandır` modu: USB'yi `CAYABOOT` (512 MB FAT32, gizli, boot sistemi) ve `CAYAISO` (kalan alan, kullanıcı veri bölümü) olarak iki bölüme ayırır.
- Safe install modu mevcut verilere dokunmaz; sadece gerekli klasörleri ve EFI payload'ını kopyalar.
- Intel RST/VMD, RAID, NVMe ve chipset storage sürücülerini WinPE başlangıcında yükleme desteği.
- Ürün anahtarı ekranını kilitlemeyen `Autounattend.xml` akışı.

---

## Proje Yapısı

```
CaYaISOBoot/
├── src/
│   ├── CaYaISOBoot.App/        # .NET 8 WPF installer uygulaması
│   ├── CaYaISOBoot.Core/       # Paylaşılan modeller, sabitler ve arayüzler
│   ├── CaYaISOBoot.Disk/       # USB disk algılama, güvenlik denetimi ve kurulum
│   ├── CaYaISOBoot.ISO/        # ISO/IMG/EFI tarama ve analiz
│   ├── CaYaISOBoot.Logging/    # Dosya ve UI loglama altyapısı
│   └── CaYaISOBoot.Tests/      # Birim testleri
├── boot/
│   └── CaYaNativeBootEngine/   # EDK2 ile derlenen UEFI uygulaması (C)
├── winpe/
│   └── CaYaWinPeBridge/        # WinPE içinde ISO takma ve sürücü yükleme (C)
├── installer_payload/
│   └── EFI/BOOT/BOOTX64.EFI   # USB'ye kopyalanacak EFI payload
└── docs/                       # Mimari, güvenlik, build ve uyumluluk dokümanları
```

---

## Bileşenler

### CaYaISOBoot.App
.NET 8 WPF tabanlı installer uygulaması. MVVM mimarisiyle inşa edilmiştir.
Disk listesi, detay paneli, ilerleme çubuğu ve log paneli içerir.
Async komutlar ve iki aşamalı onay akışı ile güvenli kurulum sunar.

### CaYaNativeBootEngine
EDK2 altyapısıyla derlenen, tamamen özgün UEFI uygulaması. Firmware tarafından
`/EFI/BOOT/BOOTX64.EFI` olarak yüklenir. Boot engine:

1. `Loaded Image Protocol` ile kendi yüklendiği device handle'ı bulur.
2. `Simple File System Protocol` üzerinden USB kök dizinini ve `ISO/` klasörünü tarar.
3. `.iso`, `.img`, `.efi` girişlerini belleğe `BOOT_ENTRY` listesi olarak alır.
4. Kullanıcıya klavyeyle gezilen bir menü çizer.
5. Seçime göre `LoadImage/StartImage` (EFI) veya ISO9660/El Torito zinciri başlatır.
6. Windows ISO seçiminde WinPE bridge akışını hazırlar.

### CaYaWinPeBridge
Windows kurulum medyasının WinPE aşamasında kaybolmamasını önleyen köprü bileşeni.
Seçilen ISO yolunu cache'den okur, Windows Virtual Disk API ile read-only takar,
`/CaYaISOBoot/drivers/winpe/` ve `/$WinPEDriver$/` altındaki `.inf` paketlerini
recursive tarayarak depolama sürücülerini yüklemeyi dener.

---

## USB Klasör Yapısı

**Clean install (`USB'yi Yapılandır`) sonrası:**

```
CAYABOOT (FAT32, 512 MB, gizli)
  /EFI/BOOT/BOOTX64.EFI
  /Autounattend.xml
  /CaYaISOBoot/system/
  /CaYaISOBoot/config/
  /CaYaISOBoot/logs/
  /CaYaISOBoot/cache/
  /CaYaISOBoot/drivers/winpe/

CAYAISO (kalan alan, kullanıcı veri bölümü)
  /                  ← ISO/IMG/EFI dosyaları doğrudan buraya
  /ISO/              ← veya bu klasöre kopyalanabilir
```

**Safe install sonrası:** Aynı klasör yapısı mevcut tek bölüm içinde oluşturulur.

---

## Kurulum ve Derleme

### Gereksinimler

- Windows 10/11
- .NET SDK 8 veya üzeri
- WPF destekli Windows Desktop Runtime
- UEFI boot engine için: EDK2 build ortamı ve MSVC x64 araçları

### Windows Installer

```powershell
# Derleme
dotnet build CaYaISOBoot.sln -c Release

# Çalıştırma
dotnet run --project src/CaYaISOBoot.App/CaYaISOBoot.App.csproj
```

Gerçek kurulum yapılabilmesi için şu dosyaların mevcut olması gerekir:

```
installer_payload/EFI/BOOT/BOOTX64.EFI
installer_payload/CaYaISOBoot/winpe/CaYaWinPeBridge.exe
```

Bu dosyalar, MSVC x64 araçları kurulu olduğunda `src/CaYaISOBoot.App` derlenirken otomatik üretilir.

### UEFI Boot Engine (EDK2)

EDK2 dizin yapısı içinde `CaYaISOBootPkg/CaYaNativeBootEngine/` olarak konumlandırın,
`dsc` dosyasına component ekleyin ve derleyin. Üretilen EFI dosyasını
`installer_payload/EFI/BOOT/BOOTX64.EFI` konumuna kopyalayın.

Ayrıntılı talimatlar: [docs/Build.md](docs/Build.md)

---

## Güvenlik Modeli

- Secure Boot bypass, imza atlatma veya firmware manipülasyonu **yoktur**.
- `C:` sürücüsü ve Windows sistem diski kurulum hedefi olamaz.
- Dahili SATA/NVMe/SCSI diskler liste dışı bırakılır.
- Safe install mevcut kullanıcı verilerini **silmez**.
- `USB'yi Yapılandır` modu yalnızca iki aşamalı onaydan sonra diski yeniden bölümlendirir.
- EFI payload eksikse installer başarılı kurulum raporu **vermez**.
- WinPE bridge yalnızca kullanıcının seçtiği ISO yolunu kullanır; sahte başarı üretmez.

Ayrıntılar: [docs/Security.md](docs/Security.md)

---

## Desteklenen Dosya Türleri

| Tür | Davranış |
|-----|----------|
| `.efi` | UEFI `LoadImage` / `StartImage` ile doğrudan başlatılır |
| `.iso` | ISO9660/El Torito zinciri; Windows ISO'ları için WinPE bridge devreye girer |
| `.img` | Raw image handler — MVP'de henüz desteklenmez |

---

## Uyumluluk

- **İlk hedef:** Secure Boot kapalı UEFI sistemleri.
- **Depolama sürücüleri:** Intel RST/VMD, RAID, NVMe veya chipset storage paketleri
  `CaYaISOBoot/drivers/winpe/` altına açılmış `.inf/.sys/.cat` olarak konabilir.
  `$WinPEDriver$` klasörü de desteklenir.
- **Windows Setup ürün anahtarı:** `ProductKey/WillShowUI=Always` sayesinde
  otomatik doldurulmaz; kullanıcı kendi seçimini yapar.

Ayrıntılar: [docs/ISOCompatibility.md](docs/ISOCompatibility.md)

---

## Yol Haritası

| Aşama | Kapsam |
|-------|--------|
| MVP 1 | Windows installer, güvenli kurulum, EFI payload, UEFI text menü, WinPE bridge cache akışı |
| MVP 2 | ISO9660 okuyucu, Virtual Disk API mount, WinPE sürücü yükleme, ISO metadata cache |
| MVP 3 | El Torito parser, Linux kernel/initrd tespiti, Ubuntu/Debian/Fedora/Kali profil sistemi |
| MVP 4 | Çoklu Windows sürümü uyumluluk doğrulaması, BCD/WIM/ESD tespiti, hata raporlama |
| MVP 5 | Legacy BIOS mimarisi (planlama aşamasında) |

Ayrıntılar: [docs/Roadmap.md](docs/Roadmap.md)

---

## Lisans

Bu proje [MIT Lisansı](LICENSE) ile lisanslanmıştır.
