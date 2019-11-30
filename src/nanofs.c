/* Nano FS */
#include <string.h>
#include "nanofs.h"
#include "nanofs_internal.h"


nanofs_t nanofs = {0, 0, 0, 0, 0};

enum {
    NANOFS_PAGE_STATUS_ADDR = 0,
    NANOFS_FILENAME_OFFSET_ADDR = 5,
    NANOFS_FILENAME_LEN_ADDR = 1,
    NANOFS_CONTENT_LEN_OFFSET_ADDR = 2,
    NANOFS_CONTENT_LEN_ADDR = 3,
    NANOFS_CREATED_ADDR = 4,
    NANOFS_STAT_TIMES_ADDR = 6,
    NANOFS_WRITE_TIMES_ADDR = 7,
    NANOFS_READ_TIMES_ADDR = 8,
} nanofs_addr_type;

void nanofs_log_int(const char *msg, int value) {
    printf(msg, value);
}

void nanofs_log_char(const char *msg, const char *value) {
    printf(msg, value);
}

int nanofs_get_addr(uint8_t addr_type) {
    if (nanofs.status == 0) {
        return NANOFS_NOT_READY;
    }
    if (addr_type == NANOFS_PAGE_STATUS_ADDR) {
        return 0;
    }
    if (addr_type == NANOFS_FILENAME_LEN_ADDR) {
        return NANOFS_PAGE_STATUS_SIZE;
    }
    if (addr_type == NANOFS_CONTENT_LEN_OFFSET_ADDR) {
        return NANOFS_PAGE_STATUS_SIZE + NANOFS_FILENAME_LEN_SIZE;
    }
    if (addr_type == NANOFS_CONTENT_LEN_ADDR) {
        return NANOFS_PAGE_STATUS_SIZE + NANOFS_FILENAME_LEN_SIZE + NANOFS_CONTENT_OFFSET_SIZE;
    }
    if (addr_type == NANOFS_FILENAME_OFFSET_ADDR) {
        return NANOFS_PAGE_STATUS_SIZE + NANOFS_FILENAME_LEN_SIZE + NANOFS_CONTENT_OFFSET_SIZE +
               NANOFS_CONTENT_LEN_SIZE;
    }

    if (addr_type == NANOFS_CREATED_ADDR) {
        return NANOFS_PAGE_STATUS_SIZE + NANOFS_FILENAME_LEN_SIZE + NANOFS_CONTENT_OFFSET_SIZE +
               NANOFS_CONTENT_LEN_SIZE + NANOFS_FILENAME_SIZE;
    }
    if (addr_type == NANOFS_STAT_TIMES_ADDR) {
        return NANOFS_CONTENT_OFFSET - NANOFS_STAT_TIMES_MAX;
    }
    if (addr_type == NANOFS_WRITE_TIMES_ADDR) {
        return NANOFS_CONTENT_OFFSET - NANOFS_STAT_TIMES_MAX - NANOFS_WRITE_TIMES_MAX;
    }
    if (addr_type == NANOFS_READ_TIMES_ADDR) {
        return NANOFS_CONTENT_OFFSET - NANOFS_STAT_TIMES_MAX - NANOFS_READ_TIMES_MAX;
    }
}

nanofs_ret nanofs_stat(const char *filename, nanofs_file_info_t *file_info) {
    if (nanofs.status == 0) {
        return NANOFS_NOT_READY;
    }
    int i;
    //read storage page status,
    uint8_t page_status[NANOFS_PAGE_STATUS_SIZE];
    uint8_t fnl[NANOFS_FILENAME_LEN_SIZE];
    uint8_t name[NANOFS_FILENAME_SIZE];
    uint8_t creater[NANOFS_CREATOR_SIZE];
    uint8_t created[NANOFS_CREATED_SIZE];
    int cn;
    for (i = 0; i < nanofs.page_count; i++) {
        cn = nanofs_native_read_bytes(nanofs.device, nanofs.offset + nanofs.page_size * i +
                                                     nanofs_get_addr(NANOFS_PAGE_STATUS_ADDR),
                                      NANOFS_PAGE_STATUS_SIZE, page_status);
        if (cn < NANOFS_PAGE_STATUS_SIZE) {
            return NANOFS_UNDEFINED_ERROR;
        }
        printf("nanofs.offset + nanofs.page_size * i +nanofs_get_addr(NANOFS_PAGE_STATUS_ADDR):%d.\n",
               nanofs.offset + nanofs.page_size * i +
               nanofs_get_addr(NANOFS_PAGE_STATUS_ADDR));
        printf("page_status[0]:%d.\n", page_status[0]);
        if (page_status[0] == NANOFS_FILE_STATUS_OK || page_status[0] == NANOFS_FILE_STATUS_WRITING ||
            page_status[0] == NANOFS_FILE_STATUS_UPDATING) {
            cn = nanofs_native_read_bytes(nanofs.device, nanofs.offset + nanofs.page_size * i +
                                                         nanofs_get_addr(NANOFS_FILENAME_LEN_ADDR),
                                          NANOFS_FILENAME_LEN_SIZE, fnl);
            if (cn < NANOFS_FILENAME_LEN_SIZE) {
                return NANOFS_UNDEFINED_ERROR;
            }
            cn = nanofs_native_read_bytes(nanofs.device, nanofs.offset + nanofs.page_size * i +
                                                         nanofs_get_addr(NANOFS_FILENAME_OFFSET_ADDR),
                                          fnl[0], name);
            int cmp_ret = strncmp(filename, name, cn);
            printf("cmp_ret:%d.\n", cmp_ret);
            printf("cn:%d.\n", cn);
            printf("filename:%s.\n", filename);
            printf("name:%s.\n", (char *) name);
            if (!cmp_ret) {
                file_info->file_len = fnl[0];
                strcpy(file_info->filename, filename);
                file_info->filename_len = strlen(filename);
                return NANOFS_NO_ERROR;
            }
        } else {
            continue;
        }
    }
    return NANOFS_NOT_FOUND;
}

nanofs_ret nanofs_read(const char *filename, uint8_t *buf) {
    if (nanofs.status == 0) {
        return NANOFS_NOT_READY;
    }
    nanofs_file_info_t *nanofs_file_info;
    nanofs_ret ret = nanofs_stat(filename, nanofs_file_info);
    if (ret) {
        return ret;
    }
    return nanofs_read_by_info(nanofs_file_info, buf);
}

nanofs_ret nanofs_read_by_info(nanofs_file_info_t *nanofs_file_info, uint8_t *buf) {
    return 0;
}

nanofs_ret nanofs_write(const char *filename, uint8_t *buf) {
    if (nanofs.status == 0) {
        return NANOFS_NOT_READY;
    }
}

nanofs_ret nanofs_delete(const char *filename) {
    if (nanofs.status == 0) {
        return NANOFS_NOT_READY;
    }
}

nanofs_ret nanofs_init(void *device, int offset, uint16_t page_size, uint8_t page_count, uint8_t overwrite) {
    if (nanofs.status == 1) {
        return NANOFS_INITED;
    }
    if (offset % page_size != 0 || page_size % NANOFS_PAGE_SIZE_UNIT != 0) {
        return NANOFS_INIT_INVALID;
    }
#ifdef NANOFS_OPTIMES_ENABLED
    if (NANOFS_CONTENT_OFFSET <
        (NANOFS_PAGE_STATUS_SIZE + NANOFS_FILENAME_LEN_SIZE + NANOFS_CONTENT_OFFSET_SIZE + NANOFS_CONTENT_LEN_SIZE +
         NANOFS_CREATED_SIZE +
         NANOFS_FILENAME_SIZE + NANOFS_CREATOR_SIZE + NANOFS_STAT_TIMES_MAX + NANOFS_WRITE_TIMES_MAX +
         NANOFS_READ_TIMES_MAX)) {
        return NANOFS_INIT_HEADER_INSUFFICIENT;
    }
#endif
    nanofs.device = device;
    nanofs.offset = offset;
    nanofs.page_size = page_size;
    nanofs.page_count = page_count;
    nanofs.status = 1;
    nanofs.overwrite = overwrite;
    return NANOFS_NO_ERROR;
}
