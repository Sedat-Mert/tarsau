#include <stdio.h>
#include <string.h>
#include "tarsau.h"

static void usage(void) {
    fprintf(stderr,
        "Kullanım:\n"
        "  tarsau -b dosya1 dosya2 ... [-o cikti.sau]\n"
        "  tarsau -a arsiv.sau [dizin]\n");
}

int main(int argc, char *argv[]) {
    if (argc < 3) { usage(); return 1; }

    /* ---- ARŞIV OLUŞTURMA: tarsau -b f1 f2 ... [-o out.sau] ---- */
    if (strcmp(argv[1], "-b") == 0) {
        char *output   = DEFAULT_OUTPUT;
        char *files[MAX_FILES];
        int   n_files  = 0;

        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-o") == 0) {
                if (i + 1 < argc) output = argv[++i];
                else { fprintf(stderr, "-o parametresi eksik!\n"); return 1; }
            } else {
                if (n_files >= MAX_FILES) {
                    fprintf(stderr, "En fazla %d dosya verilebilir!\n", MAX_FILES);
                    return 1;
                }
                files[n_files++] = argv[i];
            }
        }

        if (n_files == 0) { fprintf(stderr, "Hiç dosya belirtilmedi!\n"); return 1; }
        return build_archive(n_files, files, output);
    }

    /* ---- ARŞİVİ AÇMA: tarsau -a arsiv.sau [dizin] ---- */
    if (strcmp(argv[1], "-a") == 0) {
        /* -a parametresinden sonra en fazla 2 parametre */
        if (argc - 2 > 2) {
            fprintf(stderr, "-a parametresinden sonra en fazla 2 argüman verilebilir!\n");
            return 1;
        }
        const char *archive = argv[2];
        const char *dir     = (argc == 4) ? argv[3] : NULL;
        return extract_archive(archive, dir);
    }

    usage();
    return 1;
}