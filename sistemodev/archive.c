#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <libgen.h>
#include "tarsau.h"

/* Dosyanın metin dosyası olup olmadığını kontrol eder.
   Basit kural: NULL bayt yoksa metin kabul edilir. */
static int is_text_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return 0;
    int c;
    while ((c = fgetc(f)) != EOF) {
        if (c == '\0') { fclose(f); return 0; }
    }
    fclose(f);
    return 1;
}

int build_archive(int file_count, char *files[], const char *output) {
    if (file_count > MAX_FILES) {
        fprintf(stderr, "Hata: En fazla %d dosya arşivlenebilir!\n", MAX_FILES);
        return 1;
    }

    FileEntry entries[MAX_FILES];
    long total_size = 0;

    /* --- Dosyaları doğrula ve meta bilgileri topla --- */
    for (int i = 0; i < file_count; i++) {
        struct stat st;
        if (stat(files[i], &st) != 0) {
            fprintf(stderr, "%s giriş dosyasının formatı uyumsuzdur!\n", files[i]);
            return 1;
        }
        if (!S_ISREG(st.st_mode)) {
            fprintf(stderr, "%s giriş dosyasının formatı uyumsuzdur!\n", files[i]);
            return 1;
        }
        if (!is_text_file(files[i])) {
            fprintf(stderr, "%s giriş dosyasının formatı uyumsuzdur!\n", files[i]);
            return 1;
        }

        total_size += st.st_size;
        if (total_size > MAX_SIZE_BYTES) {
            fprintf(stderr, "Hata: Toplam dosya boyutu 200 MB'ı geçemez!\n");
            return 1;
        }

        strncpy(entries[i].path, files[i], MAX_PATH - 1);
        /* basename() bazı sistemlerde girdi stringini değiştirir, kopya alalım */
        char tmp[MAX_PATH];
        strncpy(tmp, files[i], MAX_PATH - 1);
        strncpy(entries[i].name, basename(tmp), MAX_PATH - 1);
        entries[i].perms = st.st_mode & 0777;
        entries[i].size  = st.st_size;
    }

    /* --- İçerik (organizasyon) bölümünü oluştur --- */
    /* Format: |ad,izin,boyut|ad,izin,boyut|...
       Örnek:  |t1,644,1024|t2,755,2048|                */
    char header_body[65536] = {0};   /* 64 KB yeterliden fazla */
    for (int i = 0; i < file_count; i++) {
        char rec[1024];
        snprintf(rec, sizeof(rec), "|%s,%o,%ld",
                 entries[i].name, (unsigned)entries[i].perms, entries[i].size);
        strncat(header_body, rec, sizeof(header_body) - strlen(header_body) - 1);
    }
    /* Son '|' kapanışı */
    strncat(header_body, "|", sizeof(header_body) - strlen(header_body) - 1);

    long header_body_len = (long)strlen(header_body);

    /* --- Arşiv dosyasını yaz --- */
    FILE *out = fopen(output, "wb");
    if (!out) {
        perror(output);
        return 1;
    }

    /* İlk 10 bayt: içerik bölümü boyutu (sağa dayalı, solda boşluk) */
    fprintf(out, "%10ld", header_body_len);

    /* İçerik bölümü */
    fwrite(header_body, 1, header_body_len, out);

    /* Dosya içeriklerini art arda yaz */
    for (int i = 0; i < file_count; i++) {
        FILE *in = fopen(entries[i].path, "rb");
        if (!in) {
            fprintf(stderr, "%s açılamadı!\n", entries[i].path);
            fclose(out);
            return 1;
        }
        char buf[4096];
        size_t n;
        while ((n = fread(buf, 1, sizeof(buf), in)) > 0)
            fwrite(buf, 1, n, out);
        fclose(in);
    }

    fclose(out);
    printf("Dosyalar birleştirildi.\n");
    return 0;
}