# ISO Uyumluluk Notları

## Güncel durum

Boot motoru ISO dosyalarını ISO9660/El Torito üzerinden zincirler. Windows
kurulum ISO'ları için ek olarak WinPE aşamasında çalışan CaYaWinPeBridge
aracı kurulur; bu araç seçilen ISO'yu Windows'un kendi Virtual Disk API'siyle
takar. Bu, Windows Setup'ın kurulum medyasını Ventoy benzeri şekilde tekrar
gorebilmesi içindir.

Windows kurulumu başlarken USB köküne yazılan `Autounattend.xml` sadece
CaYaWinPeBridge'i ve WinPE sürücü yolunu devreye almak içindir. Ürün anahtarı
otomatik doldurulmaz; Windows Setup'ın kendi anahtar/edition ekranını
göstermesi için `ProductKey/WillShowUI=Always` kullanılır.

Ek depolama sürücüleri için klasör:

```text
/CaYaISOBoot/drivers/winpe/
/$WinPEDriver$/
```

Intel RST/VMD, RAID, NVMe veya chipset storage paketleri bu klasöre
`.inf/.sys/.cat` olarak açılmış halde konabilir. CaYaWinPeBridge bu klasörü
recursive tarar ve `.inf` dosyalarını WinPE başlangıcında yüklemeyi dener.
`/CaYaISOBoot/drivers/winpe/` ayrıca Windows Setup DriverPaths için ilan
edilir; `/$WinPEDriver$/` ise Windows Setup'ın kendi standart tarama yoludur.

## Dosya türleri

- `.efi`: Doğrudan UEFI image olarak başlatma denenir.
- `.iso`: ISO handler'a aktarılır; Windows ISO'ları için WinPE bridge devreye girer.
- `.img`: Raw image handler'a aktarılır; MVP'de desteklenmez.

## Sonraki uyumluluk hedefleri

- Linux live ISO'larında kernel/initrd ve boot parametrelerini üretme.
- Windows ISO bridge için Windows 7/WinPE eski sürümlerinde ImDisk benzeri
  alternatif mount yolu ekleme.
- Hata durumlarını kullanıcıya net ve teknik olarak doğru gösterme.
