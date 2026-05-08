# tarsau - Arşivleme Programı

Bilgisayar Mühendisliği Sistem Programlama 2025-2026 Bahar Dönemi Projesi

## Açıklama

`tarsau`, Linux ortamında C diliyle geliştirilmiş, sıkıştırma yapmayan bir arşivleme programıdır. `tar` ve `zip` gibi çalışır; metin dosyalarını tek bir `.sau` arşiv dosyasında birleştirir ve geri açar.

## Derleme

```bash
make
```

## Kullanım

### Arşiv Oluşturma (`-b`)
```bash
./tarsau -b dosya1.txt dosya2.txt dosya3.txt -o arsiv.sau
```
- `-o` belirtilmezse çıktı dosyası varsayılan olarak `a.sau` olur.

### Arşivi Açma (`-a`)
```bash
./tarsau -a arsiv.sau hedef_dizin
```
- Dizin belirtilmezse dosyalar geçerli dizine açılır.
- Hedef dizin yoksa otomatik oluşturulur.

## Kısıtlamalar

- Yalnızca metin (ASCII) dosyaları arşivlenebilir
- En fazla 32 dosya
- Toplam boyut 200 MB'ı geçemez
- Dosya izinleri (rwx) korunur

## Arşiv Dosyası Formatı (`.sau`)

```
[10 bayt: içerik boyutu][|dosyaadı,izin,boyut|dosyaadı,izin,boyut|...][dosya içerikleri]
```

Örnek:
```
        43|t1.txt,644,14|t2.txt,644,17|merhaba dünyabu ikinci dosya
```

## Örnek

```bash
echo "merhaba" > t1.txt
echo "dünya" > t2.txt

./tarsau -b t1.txt t2.txt -o test.sau
# Dosyalar birleştirildi.

./tarsau -a test.sau cikti
# cikti dizininde t1.txt, t2.txt dosyaları açıldı.
```

## Dosya Yapısı

```
├── tarsau.c      # main, argüman işleme
├── tarsau.h      # sabitler, struct tanımları
├── archive.c     # arşiv oluşturma (-b)
├── extract.c     # arşivi açma (-a)
├── Makefile      # derleme
└── README.md
```
