/* Nano FS */
#include <string.h>
#include "nanofs/nanofs.h"
#include "nanofs_internal.h"

//TODO 目前实现了，简单的读写，需要优化实现的是：
// 1。文件update操作，目前只实现了擦除后重新写入
// 2。目前没有记录 creator 与 created
// 3。stat read write 的optimes次数存储空间已经预留，暂时没有实现
// 4。文件删除目前直接擦除

nanofs_t nanofs = {0, 0, 0, 0, 0, 0, 0, NULL};

int array_cmp(const uint8_t *arr1, const uint8_t *arr2, uint16_t size) {
    for (uint16_t index = 0; index < size; index++) {
        if (arr1[index] != arr2[index])
            return 0;
    }
    return 1;
}

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
        return NANO_FS_NOT_READY;
    }
    if (addr_type == NANOFS_PAGE_STATUS_ADDR) {
        return 0;
    }
    if (addr_type == NANOFS_CONTENT_LEN_OFFSET_ADDR) {
        return NANO_FS_PAGE_STATUS_SIZE;
    }
    if (addr_type == NANOFS_CONTENT_LEN_ADDR) {
        return NANO_FS_PAGE_STATUS_SIZE + NANO_FS_CONTENT_OFFSET_SIZE;
    }
    if (addr_type == NANOFS_FILENAME_OFFSET_ADDR) {
        return NANO_FS_PAGE_STATUS_SIZE + NANO_FS_CONTENT_OFFSET_SIZE +
               NANO_FS_CONTENT_LEN_SIZE;
    }

    if (addr_type == NANOFS_CREATED_ADDR) {
        return NANO_FS_PAGE_STATUS_SIZE + NANO_FS_CONTENT_OFFSET_SIZE +
               NANO_FS_CONTENT_LEN_SIZE + NANO_FS_FILENAME_SIZE;
    }
    if (addr_type == NANOFS_STAT_TIMES_ADDR) {
        return NANO_FS_CONTENT_OFFSET - NANO_FS_STAT_TIMES_MAX;
    }
    if (addr_type == NANOFS_WRITE_TIMES_ADDR) {
        return NANO_FS_CONTENT_OFFSET - NANO_FS_STAT_TIMES_MAX - NANO_FS_WRITE_TIMES_MAX;
    }
    if (addr_type == NANOFS_READ_TIMES_ADDR) {
        return NANO_FS_CONTENT_OFFSET - NANO_FS_STAT_TIMES_MAX - NANO_FS_READ_TIMES_MAX;
    }
}

int nanofs_page_has_data(int status) {
    return status == NANOFS_FILE_STATUS_OK || status == NANOFS_FILE_STATUS_WRITING ||
           status == NANOFS_FILE_STATUS_UPDATING;
}

nano_fs_ret nano_fs_stat(const uint8_t *filename, uint16_t filename_len, nano_fs_file_info_t *file_info) {
    nano_fs_ret ret = nanofs_do_stat(filename, filename_len, file_info);
    if (ret == NANO_FS_NO_ERROR) {

    }
    return ret;
}

nano_fs_ret nanofs_do_stat(const uint8_t *filename, uint16_t filename_len, nano_fs_file_info_t *file_info) {
    if (nanofs.status == 0) {
        return NANO_FS_NOT_READY;
    }
    int i;
    //read storage page status,
    uint8_t page_status[NANO_FS_PAGE_STATUS_SIZE];
    uint8_t fnl[NANO_FS_FILENAME_LEN_SIZE];
    uint8_t name[NANO_FS_FILENAME_SIZE];
    uint8_t creater[NANO_FS_CREATOR_SIZE];
    uint8_t created[NANO_FS_CREATED_SIZE];
    int cn;
    for (i = 0; i < nanofs.page_count; i++) {
        cn = nano_fs_native_read_bytes(nanofs.device, nanofs.offset + nanofs.page_size * i +
                                                      nanofs_get_addr(NANOFS_PAGE_STATUS_ADDR),
                                       NANO_FS_PAGE_STATUS_SIZE, page_status);
        if (cn < NANO_FS_PAGE_STATUS_SIZE) {
            return NANO_FS_UNDEFINED_ERROR;
        }
        if (nanofs_page_has_data(page_status[0])) {
            cn = nano_fs_native_read_bytes(nanofs.device, nanofs.offset + nanofs.page_size * i +
                                                          nanofs_get_addr(NANOFS_FILENAME_OFFSET_ADDR),
                                           NANO_FS_FILENAME_LEN_SIZE, fnl);
            if (cn < NANO_FS_FILENAME_LEN_SIZE) {
                return NANO_FS_UNDEFINED_ERROR;
            }
            if (fnl[0] == 0xFF >= 64 || fnl[0] == 0) {
                continue;
            }
            cn = nano_fs_native_read_bytes(nanofs.device, nanofs.offset + nanofs.page_size * i +
                                                          nanofs_get_addr(NANOFS_FILENAME_OFFSET_ADDR) +
                                                          NANO_FS_FILENAME_LEN_SIZE,
                                           fnl[0], name);
            if (cn < fnl[0]) {
                return NANO_FS_UNDEFINED_ERROR;
            }
//            int cmp_ret = memcmp(filename, name, cn);
//            int cmp_ret_r = memcmp(filename, name, filename_len);
            if (filename_len == cn && (array_cmp(filename, name, filename_len))) {

                uint8_t content_len[NANO_FS_CONTENT_LEN_SIZE];
                cn = nano_fs_native_read_bytes(nanofs.device, nanofs.offset + nanofs.page_size * i +
                                                              nanofs_get_addr(NANOFS_CONTENT_LEN_ADDR),
                                               NANO_FS_CONTENT_LEN_SIZE, content_len);
                if (cn < NANO_FS_CONTENT_LEN_SIZE) {
                    return NANO_FS_UNDEFINED_ERROR;
                }
                uint8_t content_offset[NANO_FS_CONTENT_OFFSET_SIZE];
                cn = nano_fs_native_read_bytes(nanofs.device, nanofs.offset + nanofs.page_size * i +
                                                              nanofs_get_addr(NANOFS_CONTENT_LEN_OFFSET_ADDR),
                                               NANO_FS_CONTENT_OFFSET_SIZE, content_offset);

                if (cn < NANO_FS_CONTENT_LEN_SIZE) {
                    return NANO_FS_UNDEFINED_ERROR;
                }

                file_info->page_index = i;
                file_info->content_len = nanofs_len_func_hrev(content_len, NANO_FS_CONTENT_LEN_SIZE);
                file_info->content_offset = nanofs_len_func_hrev(content_offset, NANO_FS_CONTENT_OFFSET_SIZE);
                strcpy(file_info->filename, filename);
                file_info->filename_len = filename_len;
                return NANO_FS_NO_ERROR;
            }
        } else {
            continue;
        }
    }
    return NANO_FS_NOT_FOUND;
}

int nano_fs_read(const uint8_t *filename, uint16_t filename_len, uint8_t *buf) {
    if (nanofs.status == 0) {
        return NANO_FS_NOT_READY;
    }
    nano_fs_file_info_t nanofs_file_info;
    nano_fs_ret ret = nanofs_do_stat(filename, filename_len, &nanofs_file_info);
    if (ret) {
        return ret;
    }
    return nano_fs_read_by_info(&nanofs_file_info, buf);
}

int nano_fs_read_by_info(nano_fs_file_info_t *nanofs_file_info, uint8_t *buf) {
    return nano_fs_native_read_bytes(nanofs.device, nanofs.offset + nanofs.page_size * nanofs_file_info->page_index +
                                                    nanofs_file_info->content_offset, nanofs_file_info->content_len,
                                     buf);
}

int nanofs_do_write(uint8_t index, const uint8_t *filename, uint16_t filename_l, uint8_t *buf, uint16_t len) {
    uint8_t data[NANO_FS_PAGE_STATUS_SIZE];
    nano_fs_native_erase(nanofs.device, nanofs.offset + nanofs.page_size * index, nanofs.page_size);
    data[0] = NANOFS_FILE_STATUS_WRITING;
    nano_fs_native_write_bytes(nanofs.device, nanofs.offset + nanofs.page_size * index +
                                              nanofs_get_addr(NANOFS_PAGE_STATUS_ADDR),
                               NANO_FS_PAGE_STATUS_SIZE, data);

    uint8_t content_offset[NANO_FS_CONTENT_OFFSET_SIZE];
    nanofs_len_func_hel(content_offset, NANO_FS_CONTENT_OFFSET_SIZE, NANO_FS_CONTENT_OFFSET);
    nano_fs_native_write_bytes(nanofs.device,
                               nanofs.offset + nanofs.page_size * index +
                               nanofs_get_addr(NANOFS_CONTENT_LEN_OFFSET_ADDR),
                               NANO_FS_CONTENT_OFFSET_SIZE, content_offset);

    uint8_t content_len[NANO_FS_CONTENT_LEN_SIZE];
    nanofs_len_func_hel(content_len, NANO_FS_CONTENT_LEN_SIZE, len);
    nano_fs_native_write_bytes(nanofs.device,
                               nanofs.offset + nanofs.page_size * index +
                               nanofs_get_addr(NANOFS_CONTENT_LEN_ADDR),
                               NANO_FS_CONTENT_LEN_SIZE, content_len);

    uint8_t filename_len[NANO_FS_FILENAME_LEN_SIZE];
    nanofs_len_func_hel(filename_len, NANO_FS_FILENAME_LEN_SIZE, filename_l);
    nano_fs_native_write_bytes(nanofs.device,
                               nanofs.offset + nanofs.page_size * index +
                               nanofs_get_addr(NANOFS_FILENAME_OFFSET_ADDR),
                               NANO_FS_FILENAME_LEN_SIZE, filename_len);

    nano_fs_native_write_bytes(nanofs.device,
                               nanofs.offset + nanofs.page_size * index +
                               nanofs_get_addr(NANOFS_FILENAME_OFFSET_ADDR) + NANO_FS_FILENAME_LEN_SIZE,
                               filename_l, filename);

    nano_fs_native_write_bytes(nanofs.device,
                               nanofs.offset + nanofs.page_size * index + NANO_FS_CONTENT_OFFSET,
                               len, buf);

    data[0] = NANOFS_FILE_STATUS_OK;
    nano_fs_native_write_bytes(nanofs.device, nanofs.offset + nanofs.page_size * index +
                                              nanofs_get_addr(NANOFS_PAGE_STATUS_ADDR),
                               NANO_FS_PAGE_STATUS_SIZE, data);
    return len;
}

int nano_fs_write(const uint8_t *filename, uint16_t filename_len, uint8_t *buf, uint16_t len) {
    if (nanofs.status == 0) {
        return NANO_FS_NOT_READY;
    }

    nano_fs_file_info_t file_info;
    int ret = nanofs_do_stat(filename, filename_len, &file_info);
    if (ret == NANO_FS_NO_ERROR) {
        return nanofs_do_write(file_info.page_index, filename, filename_len, buf, len);
    } else if (ret == NANO_FS_NOT_FOUND) {
        int index;
        uint8_t data[NANO_FS_PAGE_STATUS_SIZE];
        for (index = 0; index < nanofs.page_count; index++) {
            int cn;
            cn = nano_fs_native_read_bytes(nanofs.device, nanofs.offset + nanofs.page_size * index +
                                                          nanofs_get_addr(NANOFS_PAGE_STATUS_ADDR),
                                           NANO_FS_PAGE_STATUS_SIZE, data);
            if (cn < NANO_FS_PAGE_STATUS_SIZE) {
                return NANO_FS_UNDEFINED_ERROR;
            }

            if (!nanofs_page_has_data(data[0])) {
                return nanofs_do_write(index, filename, filename_len, buf, len);
            }

        }
        return NANO_FS_INSUFFICIENT_SPACE;
    } else {
        return ret;
    }
}

int nano_fs_delete(const uint8_t *filename, uint16_t filename_len) {
    if (nanofs.status == 0) {
        return NANO_FS_NOT_READY;
    }
    nano_fs_file_info_t file_info;
    int ret = nanofs_do_stat(filename, filename_len, &file_info);

    if (ret == NANO_FS_NO_ERROR) {
        return nano_fs_native_erase(nanofs.device, nanofs.offset + nanofs.page_size * file_info.page_index,
                                    nanofs.page_size);
    } else {
        return ret;
    }
}

nano_fs_ret nano_fs_init(void *device, int offset, uint16_t page_size, uint8_t page_count, uint8_t overwrite,
                         uint8_t erase_before_write) {
    if (nanofs.status == 1) {
        return NANO_FS_INITED;
    }
    if (offset % page_size != 0 || page_size % NANO_FS_PAGE_SIZE_UNIT != 0) {
        return NANO_FS_INIT_INVALID;
    }
#ifdef NANO_FS_OPTIMES_ENABLED
    if (NANO_FS_CONTENT_OFFSET <
        (NANO_FS_PAGE_STATUS_SIZE + NANO_FS_FILENAME_LEN_SIZE + NANO_FS_CONTENT_OFFSET_SIZE + NANO_FS_CONTENT_LEN_SIZE +
         NANO_FS_CREATED_SIZE +
         NANO_FS_FILENAME_SIZE + NANO_FS_CREATOR_SIZE + NANO_FS_STAT_TIMES_MAX + NANO_FS_WRITE_TIMES_MAX +
         NANO_FS_READ_TIMES_MAX)) {
        return NANO_FS_INIT_HEADER_INSUFFICIENT;
    }
#endif
    nanofs.device = device;
    nanofs.offset = offset;
    nanofs.page_size = page_size;
    nanofs.page_count = page_count;
    nanofs.status = 1;
    nanofs.erase_before_write = erase_before_write;
    nanofs.overwrite = overwrite;
    return NANO_FS_NO_ERROR;
}
