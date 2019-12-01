/* Nano FS */
#include <string.h>
#include "nanofs.h"
#include "nanofs_internal.h"

//TODO 目前实现了，简单的读写，需要优化实现的是：
// 1。文件update操作，目前只实现了擦除后重新写入
// 2。目前没有记录 creator 与 created
// 3。stat read write 的optimes次数存储空间已经预留，暂时没有实现
// 4。文件删除目前直接擦除

nanofs_t nanofs = {0, 0, 0, 0, 0, 0, 0, NULL};

typedef enum nanofs_addr_type {
    NANOFS_PAGE_STATUS_ADDR = 0,
    NANOFS_FILENAME_OFFSET_ADDR = 5,
//    NANOFS_FILENAME_LEN_ADDR = 1,
            NANOFS_CONTENT_LEN_OFFSET_ADDR = 2,
    NANOFS_CONTENT_LEN_ADDR = 3,
    NANOFS_CREATED_ADDR = 4,
    NANOFS_STAT_TIMES_ADDR = 6,
    NANOFS_WRITE_TIMES_ADDR = 7,
    NANOFS_READ_TIMES_ADDR = 8,
};

void nanofs_len_func_hel(uint8_t *data, int arr_len, int len) {
    int index;
    for (index = arr_len; index > 0; index--) {
        data[arr_len - index] = len >> (8 * (index - 1));
    }
}

int nanofs_len_func_hrev(uint8_t *data, int arr_len) {
    int len = 0;
    int index;
    for (index = arr_len; index > 0; index--) {
        len = len + (data[arr_len - index] << (8 * (index - 1)));
    }
    return len;
}


int nanofs_get_addr(uint8_t addr_type) {
    if (nanofs.status == 0) {
        return NANOFS_NOT_READY;
    }
    if (addr_type == NANOFS_PAGE_STATUS_ADDR) {
        return 0;
    }
    if (addr_type == NANOFS_CONTENT_LEN_OFFSET_ADDR) {
        return NANOFS_PAGE_STATUS_SIZE;
    }
    if (addr_type == NANOFS_CONTENT_LEN_ADDR) {
        return NANOFS_PAGE_STATUS_SIZE + NANOFS_CONTENT_OFFSET_SIZE;
    }
    if (addr_type == NANOFS_FILENAME_OFFSET_ADDR) {
        return NANOFS_PAGE_STATUS_SIZE + NANOFS_CONTENT_OFFSET_SIZE +
               NANOFS_CONTENT_LEN_SIZE;
    }

    if (addr_type == NANOFS_CREATED_ADDR) {
        return NANOFS_PAGE_STATUS_SIZE + NANOFS_CONTENT_OFFSET_SIZE +
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

int nanofs_page_has_data(int status) {
    return status == NANOFS_FILE_STATUS_OK || status == NANOFS_FILE_STATUS_WRITING ||
           status == NANOFS_FILE_STATUS_UPDATING;
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
        if (nanofs_page_has_data(page_status[0])) {
            cn = nanofs_native_read_bytes(nanofs.device, nanofs.offset + nanofs.page_size * i +
                                                         nanofs_get_addr(NANOFS_FILENAME_OFFSET_ADDR),
                                          NANOFS_FILENAME_LEN_SIZE, fnl);
            if (cn < NANOFS_FILENAME_LEN_SIZE) {
                return NANOFS_UNDEFINED_ERROR;
            }
            if (fnl[0] == 0xFF >= 64 || fnl[0] == 0) {
                continue;
            }
            cn = nanofs_native_read_bytes(nanofs.device, nanofs.offset + nanofs.page_size * i +
                                                         nanofs_get_addr(NANOFS_FILENAME_OFFSET_ADDR) +
                                                         NANOFS_FILENAME_LEN_SIZE,
                                          fnl[0], name);
            if (cn < fnl[0]) {
                return NANOFS_UNDEFINED_ERROR;
            }
            int cmp_ret = strncmp(filename, name, cn);
            if (!cmp_ret) {

                uint8_t file_len[NANOFS_CONTENT_LEN_SIZE];
                cn = nanofs_native_read_bytes(nanofs.device, nanofs.offset + nanofs.page_size * i +
                                                             nanofs_get_addr(NANOFS_CONTENT_LEN_ADDR),
                                              NANOFS_CONTENT_LEN_SIZE, file_len);
                if (cn < NANOFS_CONTENT_LEN_SIZE) {
                    return NANOFS_UNDEFINED_ERROR;
                }
                uint8_t content_offset[NANOFS_CONTENT_OFFSET_SIZE];
                cn = nanofs_native_read_bytes(nanofs.device, nanofs.offset + nanofs.page_size * i +
                                                             nanofs_get_addr(NANOFS_CONTENT_LEN_OFFSET_ADDR),
                                              NANOFS_CONTENT_OFFSET_SIZE, content_offset);

                if (cn < NANOFS_CONTENT_LEN_SIZE) {
                    return NANOFS_UNDEFINED_ERROR;
                }

                file_info->page_index = i;
                file_info->file_len = nanofs_len_func_hrev(file_len, NANOFS_CONTENT_LEN_SIZE);
                file_info->content_offset = nanofs_len_func_hrev(content_offset, NANOFS_CONTENT_OFFSET_SIZE);
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

int nanofs_read(const char *filename, uint8_t *buf) {
    if (nanofs.status == 0) {
        return NANOFS_NOT_READY;
    }
    nanofs_file_info_t nanofs_file_info;
    nanofs_ret ret = nanofs_stat(filename, &nanofs_file_info);
    if (ret) {
        return ret;
    }
    return nanofs_read_by_info(&nanofs_file_info, buf);
}

int nanofs_read_by_info(nanofs_file_info_t *file_info, uint8_t *buf) {
    return nanofs_native_read_bytes(nanofs.device, nanofs.offset + nanofs.page_size * file_info->page_index +
                                                   file_info->content_offset, file_info->file_len, buf);
}

int nanofs_do_write(uint8_t index, uint8_t *filename, uint8_t *buf, uint16_t len) {
    uint8_t data[NANOFS_PAGE_STATUS_SIZE];
    nanofs_native_erase(nanofs.device, nanofs.offset + nanofs.page_size * index, nanofs.page_size);
    data[0] = NANOFS_FILE_STATUS_WRITING;
    nanofs_native_write_bytes(nanofs.device, nanofs.offset + nanofs.page_size * index +
                                             nanofs_get_addr(NANOFS_PAGE_STATUS_ADDR),
                              NANOFS_PAGE_STATUS_SIZE, data);

    uint8_t content_offset[NANOFS_CONTENT_OFFSET_SIZE];
    nanofs_len_func_hel(content_offset, NANOFS_CONTENT_OFFSET_SIZE, NANOFS_CONTENT_OFFSET);
    nanofs_native_write_bytes(nanofs.device,
                              nanofs.offset + nanofs.page_size * index +
                              nanofs_get_addr(NANOFS_CONTENT_LEN_OFFSET_ADDR),
                              NANOFS_CONTENT_OFFSET_SIZE, content_offset);

    uint8_t content_len[NANOFS_CONTENT_LEN_SIZE];
    nanofs_len_func_hel(content_len, NANOFS_CONTENT_LEN_SIZE, len);
    nanofs_native_write_bytes(nanofs.device,
                              nanofs.offset + nanofs.page_size * index +
                              nanofs_get_addr(NANOFS_CONTENT_LEN_ADDR),
                              NANOFS_CONTENT_LEN_SIZE, content_len);

    uint8_t filename_len[NANOFS_FILENAME_LEN_SIZE];
    nanofs_len_func_hel(filename_len, NANOFS_FILENAME_LEN_SIZE, strlen(filename));
    nanofs_native_write_bytes(nanofs.device,
                              nanofs.offset + nanofs.page_size * index +
                              nanofs_get_addr(NANOFS_FILENAME_OFFSET_ADDR),
                              NANOFS_FILENAME_LEN_SIZE, filename_len);

    nanofs_native_write_bytes(nanofs.device,
                              nanofs.offset + nanofs.page_size * index +
                              nanofs_get_addr(NANOFS_FILENAME_OFFSET_ADDR) + NANOFS_FILENAME_LEN_SIZE,
                              strlen(filename), filename);

    nanofs_native_write_bytes(nanofs.device,
                              nanofs.offset + nanofs.page_size * index + NANOFS_CONTENT_OFFSET,
                              len, buf);

    data[0] = NANOFS_FILE_STATUS_OK;
    nanofs_native_write_bytes(nanofs.device, nanofs.offset + nanofs.page_size * index +
                                             nanofs_get_addr(NANOFS_PAGE_STATUS_ADDR),
                              NANOFS_PAGE_STATUS_SIZE, data);
    return len;
}

int nanofs_write(const char *filename, uint8_t *buf, uint16_t len) {
    if (nanofs.status == 0) {
        return NANOFS_NOT_READY;
    }

    nanofs_file_info_t file_info;
    int ret = nanofs_stat(filename, &file_info);
    if (ret == NANOFS_NO_ERROR) {
        return nanofs_do_write(file_info.page_index, filename, buf, len);
    } else if (ret == NANOFS_NOT_FOUND) {
        int index;
        uint8_t data[NANOFS_PAGE_STATUS_SIZE];
        for (index = 0; index < nanofs.page_count; index++) {
            int cn;
            cn = nanofs_native_read_bytes(nanofs.device, nanofs.offset + nanofs.page_size * index +
                                                         nanofs_get_addr(NANOFS_PAGE_STATUS_ADDR),
                                          NANOFS_PAGE_STATUS_SIZE, data);
            if (cn < NANOFS_PAGE_STATUS_SIZE) {
                return NANOFS_UNDEFINED_ERROR;
            }

            if (!nanofs_page_has_data(data[0])) {
                return nanofs_do_write(index, filename, buf, len);
            }

        }
    } else {
        return ret;
    }
    return NANOFS_UNDEFINED_ERROR;
}

int nanofs_delete(const char *filename) {
    if (nanofs.status == 0) {
        return NANOFS_NOT_READY;
    }
    nanofs_file_info_t file_info;
    int ret = nanofs_stat(filename, &file_info);

    if (ret == NANOFS_NO_ERROR) {
        return nanofs_native_erase(nanofs.device, nanofs.offset + nanofs.page_size * file_info.page_index,
                                   nanofs.page_size);
    } else {
        return ret;
    }
}

nanofs_ret nanofs_init(void *device, int offset, uint16_t page_size, uint8_t page_count, uint8_t overwrite,
                       uint8_t erase_before_write) {
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
    nanofs.erase_before_write = erase_before_write;
    nanofs.overwrite = overwrite;
    return NANOFS_NO_ERROR;
}
