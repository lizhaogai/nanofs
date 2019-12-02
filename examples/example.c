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
    nanofs_init(NULL, 122 * 4 * 1024, 4 * 1024, 6, 0, 0);

    uint8_t d[300] = "111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111";

    nanofs_write("woshihaode", d, strlen(d));
    nanofs_file_info_t fileInfo = {};
    const uint8_t *filename;
    filename = "woshihaode";;
    int ret = nanofs_stat(filename, &fileInfo);

    uint8_t rd[300];
    nanofs_read(filename, rd);
    printf("nanofs value rd :%s.\n", (const char *)rd);

    printf("nanofs stat result :%d.\n", ret);
    if (ret == 0) {
        printf("nanofs stat filename :%s.\n", fileInfo.filename);
        printf("nanofs stat content_len :%d.\n", fileInfo.content_len);
        printf("nanofs content offset :%d.\n", fileInfo.content_offset);
        printf("nanofs stat filename_len :%d.\n", fileInfo.filename_len);
    }

    uint8_t dd[600] = "222222222";
    ret = nanofs_write("woshihaode", dd, strlen(dd));
    ret = nanofs_stat(filename, &fileInfo);
    printf("nanofs stat after update result :%d.\n", ret);
    if (ret == 0) {
        printf("nanofs stat filename :%s.\n", fileInfo.filename);
        printf("nanofs stat content_len :%d.\n", fileInfo.content_len);
        printf("nanofs content offset :%d.\n", fileInfo.content_offset);
        printf("nanofs stat filename_len :%d.\n", fileInfo.filename_len);

    }

    uint8_t rdd[9];
    nanofs_read(filename, rdd);
    printf("nanofs value rdd :%s.\n", rdd);

    ret = nanofs_delete(filename);
    ret = nanofs_stat(filename, &fileInfo);
    printf("nanofs stat after delete result :%d.\n", ret);
    if (ret == 0) {
        printf("nanofs stat filename :%s.\n", fileInfo.filename);
        printf("nanofs stat content_len :%d.\n", fileInfo.content_len);
        printf("nanofs content offset :%d.\n", fileInfo.content_offset);
        printf("nanofs stat filename_len :%d.\n", fileInfo.filename_len);
    }
}