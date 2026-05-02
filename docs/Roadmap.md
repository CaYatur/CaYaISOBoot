# Roadmap

## MVP 1

- Windows installer ile güvenli kurulum.
- C:, sistem diski ve dahili disk koruması.
- USB klasör yapısı.
- EFI payload kopyalama.
- UEFI text menü.
- Dinamik `.iso`, `.img`, `.efi` tarama.
- `.efi` chainload.
- Windows ISO seçimi için WinPE bridge cache akışı.

## MVP 2

- ISO9660 okuyucu.
- ISO içinde `/EFI/BOOT/BOOTX64.EFI` tespiti.
- Windows ISO'yu WinPE içinde Virtual Disk API ile read-only takma.
- WinPE depolama sürücülerini `.inf` paketlerinden yükleme.
- Ürün anahtarı ekranını kilitlemeyen Autounattend akışı.
- ISO metadata cache.
- Installer tarafında ISO analiz aracı.

## MVP 3

- El Torito boot catalog parser.
- Linux kernel/initrd tespiti.
- Ubuntu, Debian, Fedora, Kali profil sistemi.
- Boot parametre üretimi.

## MVP 4

- Windows ISO uyumluluğunu daha fazla sürümle doğrulama.
- `bootmgfw.efi`, BCD, `install.wim` / `install.esd` tespitlerini UI'a taşıma.
- Windows PE/installer hata raporlarını uygulama loglarıyla birleştirme.
- Büyük dosya ve çoklu bölüm stratejisi.

## MVP 5

- Legacy BIOS mimari tasarımı.
- 16-bit stage1, stage2 loader, disk okuma ve text menü.
- FAT/exFAT/NTFS okuyucu planları.
- Bu aşama MVP kapsamında uygulanmaz.
