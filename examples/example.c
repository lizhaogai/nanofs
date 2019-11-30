#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include "example-buffer-storage.c"
#include "../src/nanofs.h"
#include "../src/nanofs.c"

struct {
    char *d;
} t;

int main() {
    buffer_storage_init();
    nanofs_init(NULL, 122 * 4 * 1024, 4 * 1024, 6, 0);
    nanofs_file_info_t *fileInfo;
    const uint8_t *filename;
    filename = "test";;
    int ret = nanofs_stat(filename, fileInfo);
    printf("nanofs stat result :%d.\n", ret);
}