# Build Talimatları

## Windows Installer

Gerekenler:

- Windows 10/11
- .NET SDK 8 veya daha yeni SDK
- WPF destekli Windows Desktop runtime

Komutlar:

```powershell
dotnet build CaYaISOBoot.sln -c Release -p:RestoreUseStaticGraphEvaluation=true -m:1
```

Çalıştırma:

```powershell
dotnet run --project src/CaYaISOBoot.App/CaYaISOBoot.App.csproj
```

Installer'ın gerçek kurulum yapabilmesi için şu dosya mevcut olmalıdır:

```text
installer_payload/EFI/BOOT/BOOTX64.EFI
installer_payload/CaYaISOBoot/winpe/CaYaWinPeBridge.exe
```

Bu dosyalar artık `src/CaYaISOBoot.App` build edilirken MSVC ile otomatik üretilir.
Otomatik build için Visual Studio / Build Tools içinde MSVC x64 araçları kurulu
olmalıdır.

## USB'yi Yapılandır modu

`USB'yi Yapılandır` modu seçilen USB fiziksel diskini yeniden bölümlendirir:

- 512 MB FAT32 `CAYABOOT`: CaYa boot sistemi ve `BOOTX64.EFI`.
- Kalan alan `CAYAISO`: sistem dosyası yazılmayan, kullanıcının ISO/IMG/EFI
  dosyalarını kopyalayacağı boş veri bölümü.

Bu işlem verileri siler. Uygulama:

- İlk açılışta disk seçmez.
- Fiziksel disk numarasını doğrular.
- Windows sistem diskiyle aynı fiziksel diski engeller.
- İki aşamalı onay alır (bilgilendirme + son Evet/Hayır onayı).

## UEFI Boot Engine

Gerekenler:

- EDK2 build ortamı
- Visual Studio Build Tools veya desteklenen GCC/Clang toolchain
- `edksetup` ve `build` komutları hazırlanmış olmalı

Önerilen EDK2 yerleştirme:

```text
edk2/
  CaYaISOBootPkg/
    CaYaNativeBootEngine/
      main.c
      ...
      CaYaNativeBootEngine.inf
```

EDK2 `dsc` dosyasına component olarak ekleyin:

```text
[Components]
  CaYaISOBootPkg/CaYaNativeBootEngine/CaYaNativeBootEngine.inf
```

Derleme sonunda üretilen EFI dosyasını şu konuma kopyalayın:

```text
installer_payload/EFI/BOOT/BOOTX64.EFI
```

## Test sırası

1. `dotnet build` hatasız bitmeli.
2. Uygulama dahili diskleri listelememeli.
3. USB bellek seçildiğinde detaylar doğru gösterilmeli.
4. Safe install iki onaydan sonra klasörleri oluşturmalı.
5. EFI payload yoksa kurulum başarısız sayılmalı.
6. Payload varken USB'de `/EFI/BOOT/BOOTX64.EFI` oluşmalı.
7. QEMU/OVMF veya gerçek UEFI makinede menü açılmalı.
8. CAYAISO köküne veya kullanıcının oluşturduğu `/ISO` klasörüne konan
   dosyalar listelenmeli. Eski/safe install uyumluluğu için `/CaYaISOBoot/ISO`
   da taranır.
9. `.efi` seçimi `LoadImage` / `StartImage` denemeli.
10. Windows `.iso` seçiminde `CaYaISOBoot/cache/selected-windows-iso.txt`
    oluşmalı.
11. Windows Setup başlarken ürün anahtarı ekranı kilitlenmemeli; kullanıcı
    kendi anahtarını girebilmeli veya ISO destekliyorsa anahtarsız ilerleyebilmeli.
12. `CaYaISOBoot/drivers/winpe/` veya `$WinPEDriver$` altına konan açılmış
    `.inf` depolama sürücüleri WinPE başlangıcında denenmeli.
