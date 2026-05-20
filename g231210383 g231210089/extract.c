#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "tarsau.h"

/* Gerekirse dizini (ve üst dizinleri) oluşturur */
static int make_dir(const char *path) {
    char tmp[MAX_PATH];
    strncpy(tmp, path, MAX_PATH - 1);
    /* Sondaki '/' varsa sil */
    size_t len = strlen(tmp);
    if (len && tmp[len-1] == '/') tmp[len-1] = '\0';

    /* Her parçayı tek tek oluştur */
    for (char *p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            mkdir(tmp, 0755);
            *p = '/';
        }
    }
    mkdir(tmp, 0755);
    return 0;
}

int extract_archive(const char *archive, const char *dir) {
    FILE *f = fopen(archive, "rb");
    if (!f) {
        fprintf(stderr, "Arşiv dosyası uygunsuz veya bozuk!\n");
        return 1;
    }

    /* --- İlk 10 bayt: içerik bölümü boyutu --- */
    char size_buf[HEADER_SIZE_LEN + 1] = {0};
    if (fread(size_buf, 1, HEADER_SIZE_LEN, f) != HEADER_SIZE_LEN) {
        fprintf(stderr, "Arşiv dosyası uygunsuz veya bozuk!\n");
        fclose(f); return 1;
    }
    long header_len = atol(size_buf);
    if (header_len <= 0) {
        fprintf(stderr, "Arşiv dosyası uygunsuz veya bozuk!\n");
        fclose(f); return 1;
    }

    /* --- İçerik bölümünü oku --- */
    char *header = malloc(header_len + 1);
    if (!header) { fclose(f); return 1; }
    if (fread(header, 1, header_len, f) != (size_t)header_len) {
        fprintf(stderr, "Arşiv dosyası uygunsuz veya bozuk!\n");
        free(header); fclose(f); return 1;
    }
    header[header_len] = '\0';

    /* --- Kayıtları ayrıştır: |ad,izin,boyut|ad,izin,boyut| --- */
    FileEntry entries[MAX_FILES];
    int count = 0;

    char *token = strtok(header, "|");
    while (token && count < MAX_FILES) {
        if (strlen(token) == 0) { token = strtok(NULL, "|"); continue; }
        /* token = "ad,izin,boyut" */
        char name[MAX_PATH]; unsigned perms; long size;
        if (sscanf(token, "%511[^,],%o,%ld", name, &perms, &size) != 3) {
            fprintf(stderr, "Arşiv dosyası uygunsuz veya bozuk!\n");
            free(header); fclose(f); return 1;
        }
        strncpy(entries[count].name, name, MAX_PATH - 1);
        entries[count].perms = (mode_t)perms;
        entries[count].size  = size;
        count++;
        token = strtok(NULL, "|");
    }
    free(header);

    /* --- Hedef dizini oluştur --- */
    char target[MAX_PATH] = ".";
    if (dir && strlen(dir) > 0)
        strncpy(target, dir, MAX_PATH - 1);
    make_dir(target);

    /* --- Dosyaları çıkar --- */
    char buf[4096];
    for (int i = 0; i < count; i++) {
        char out_path[MAX_PATH * 2];
        snprintf(out_path, sizeof(out_path), "%s/%s", target, entries[i].name);

        FILE *out = fopen(out_path, "wb");
        if (!out) {
            fprintf(stderr, "%s oluşturulamadı!\n", out_path);
            fclose(f); return 1;
        }

        long remaining = entries[i].size;
        while (remaining > 0) {
            size_t to_read = (remaining < (long)sizeof(buf)) ? remaining : sizeof(buf);
            size_t n = fread(buf, 1, to_read, f);
            if (n == 0) break;
            fwrite(buf, 1, n, out);
            remaining -= n;
        }
        fclose(out);

        /* Orijinal izinleri geri yükle */
        chmod(out_path, entries[i].perms);
    }

    fclose(f);

    if (strlen(target) > 1)
        printf("%s dizininde", target);
    else
        printf("Geçerli dizinde");

    printf(" ");
    for (int i = 0; i < count; i++) {
        printf("%s", entries[i].name);
        if (i < count - 1) printf(", ");
    }
    printf(" dosyaları açıldı.\n");
    return 0;
}