#ifndef TARSAU_H
#define TARSAU_H

#include <sys/types.h>
#include <sys/stat.h>

#define MAX_FILES       32
#define MAX_PATH        512
#define MAX_SIZE_BYTES  (200LL * 1024 * 1024)  /* 200 MB */
#define HEADER_SIZE_LEN 10   /* ilk 10 bayt: içerik bölümü boyutu */
#define DEFAULT_OUTPUT  "a.sau"

/* Bir dosyaya ait meta bilgiler */
typedef struct {
    char path[MAX_PATH];   /* orijinal yol */
    char name[MAX_PATH];   /* sadece dosya adı */
    mode_t perms;          /* rwx izinleri */
    long  size;            /* bayt cinsinden boyut */
} FileEntry;

/* -b : arşiv oluştur */
int build_archive(int file_count, char *files[], const char *output);

/* -a : arşivi aç */
int extract_archive(const char *archive, const char *dir);

#endif