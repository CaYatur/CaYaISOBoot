# Güvenlik Modeli

## Temel kurallar

- Secure Boot bypass, imza atlatma veya firmware manipülasyonu yoktur.
- Safe install kullanıcı verisini silmez; USB'yi Yapılandır modu ise seçilen
  USB'yi iki aşamalı onaydan sonra yeniden bölümlendirir.
- Sistem diski, `C:` ve Windows kök sürücüsü kurulum hedefi olamaz.
- Dahili diskler disk listesine dahil edilmez.
- EFI payload yoksa installer başarılı kurulum raporu vermez.

## Disk koruma

Installer her aday disk için şu kontrolleri yapar:

- Volume hazır mı?
- Drive letter sistem kök sürücüsü mü?
- Bus tipi USB mi veya media removable mı?
- Dosya sistemi okunabiliyor mu?
- Kapasite beklenenden çok büyük mü?

Engelleyen bulgu varsa kurulum komutu çalışmaz.

## Secure Boot

İlk sürüm Secure Boot kapalı UEFI sistemleri hedefler. Secure Boot desteği
için sonraki sürümde EFI binary imzalama, anahtar dağıtımı ve kullanıcıya
doğru trust modeli dokümante edilmelidir.

## ISO boot güvenliği

Windows ISO'ları için WinPE bridge sadece kullanıcının menü üzerinden seçtiği
ISO yolunu cache'e yazar ve Windows PE içinde aynı ISO'yu read-only olarak
takar. Bridge sürücü yükleme için yalnızca USB'deki açılmış `.inf` paketlerini
dener; imza atlatma veya Secure Boot bypass yapmaz. Desteklenmeyen ISO
yollarında sahte başarı üretmez.
