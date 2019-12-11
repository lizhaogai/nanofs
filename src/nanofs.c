/* Nano FS */
#include <string.h>
#include "nanofs/nanofs.h"
#include "nanofs_internal.h"

//TODO 目前实现了，简单的读写，需要优化实现的是：
// 1。文件update操作，目前只实现了擦除后重新写入
// 2。目前没有记录 creator 与 created
// 3。stat read write 的optimes次数存储空间已经预留，暂时没有实现
// 4。文件删除目前直接擦除

nano_fs_t nano_fs = {0, 0, 0, 0, 1, 0, NULL};

enum {
    NANO_FS_PAGE_STATUS_ADDR = 0,
    NANO_FS_CONTENT_OFFSET_ADDR = 2,
    NANO_FS_CREATED_ADDR = 4,
    NANO_FS_FILENAME_OFFSET_ADDR = 5,
    NANO_FS_STAT_TIMES_ADDR = 6,
    NANO_FS_WRITE_TIMES_ADDR = 7,
    NANO_FS_READ_TIMES_ADDR = 8,
    NANO_FS_CREATOR_ADDR = 9,
};

void nano_fs_len_func_hel(uint8_t *data, int arr_len, int len) {
    int index;
    for (index = arr_len; index > 0; index--) {
        data[arr_len - index] = len >> (8 * (index - 1));
    }
}

int nano_fs_len_func_hre(uint8_t *data, int arr_len) {
    int len = 0;
    int index;
    for (index = arr_len; index > 0; index--) {
        len = len + (data[arr_len - index] << (8 * (index - 1)));
    }
    return len;
}


uint16_t nano_fs_get_addr(uint8_t addr_type) {
    if (nano_fs.status == 0) {
        return NANO_FS_NOT_READY;
    }
    if (addr_type == NANO_FS_PAGE_STATUS_ADDR) {
        return 0;
    }

    if (addr_type == NANO_FS_FILENAME_OFFSET_ADDR) {
        return NANO_FS_PAGE_STATUS_SIZE;
    }

    if (addr_type == NANO_FS_CREATED_ADDR) {
        return NANO_FS_PAGE_STATUS_SIZE +
               NANO_FS_FILENAME_SIZE;
    }
    if (addr_type == NANO_FS_CREATOR_ADDR) {
        return NANO_FS_PAGE_STATUS_SIZE +
               NANO_FS_FILENAME_SIZE + NANO_FS_CREATED_SIZE;
    }
    if (addr_type == NANO_FS_STAT_TIMES_ADDR) {
        return NANO_FS_PAGE_STATUS_SIZE +
               NANO_FS_FILENAME_SIZE + NANO_FS_CREATED_SIZE + NANO_FS_CREATOR_SIZE;
    }
    if (addr_type == NANO_FS_WRITE_TIMES_ADDR) {
        return NANO_FS_PAGE_STATUS_SIZE +
               NANO_FS_FILENAME_SIZE + NANO_FS_CREATED_SIZE + NANO_FS_CREATOR_SIZE +
               NANO_FS_STAT_TIMES_MAX;
    }
    if (addr_type == NANO_FS_READ_TIMES_ADDR) {
        return NANO_FS_PAGE_STATUS_SIZE +
               NANO_FS_FILENAME_SIZE + NANO_FS_CREATED_SIZE + NANO_FS_CREATOR_SIZE +
               NANO_FS_STAT_TIMES_MAX + NANO_FS_WRITE_TIMES_MAX;
    }
//    if (addr_type == NANO_FS_CONTENT_OFFSET_ADDR) {
//        return NANO_FS_PAGE_STATUS_SIZE +
//               NANO_FS_FILENAME_SIZE + NANO_FS_CREATED_SIZE + NANO_FS_CREATOR_SIZE +
//               NANO_FS_STAT_TIMES_MAX + NANO_FS_WRITE_TIMES_MAX + NANO_FS_READ_TIMES_MAX;
//    }
    return INT16_MAX;
}

int nano_fs_page_has_data(int status) {
    return status == NANOFS_FILE_STATUS_OK || status == NANOFS_FILE_STATUS_WRITING ||
           status == NANOFS_FILE_STATUS_UPDATING;
}

int nano_fs_record_op(nano_fs_file_info_t *file_info, int type, size_t times) {
    uint16_t start = nano_fs_get_addr(type);
    size_t size;
    if (type == NANO_FS_STAT_TIMES_ADDR) {
        size = NANO_FS_STAT_TIMES_MAX;
    } else if (type == NANO_FS_WRITE_TIMES_ADDR) {
        size = NANO_FS_WRITE_TIMES_MAX;
    } else {
        size = NANO_FS_READ_TIMES_MAX;
    }

    uint8_t data[32];
    nano_fs_native_read_bytes(nano_fs.device, nano_fs.offset + nano_fs.page_size * file_info->page_index + start, size,
                              data);
//    for (size_t t = 0; t < times; t++) {
//        for (size_t s = 0; s < size; s++) {
//            if (data[s]) {
//                uint8_t rd[] = {data[s] >> 1};
//                nano_fs_native_write_bytes(nano_fs.device,
//                                           nano_fs.offset + nano_fs.page_size * file_info->page_index + start + s, 1,
//                                           rd);
//                return 0;
//            }
//        }
//    }
    size_t af = 0;
    for (size_t t = 0; t < times; t++) {
        for (size_t s = 0; s < size; s++) {
            if (data[s]) {
                data[s] = data[s] >> 1;
                af = t + 1;
                break;
            }
        }
    }

    if (af > 0) {
        nano_fs_native_write_bytes(nano_fs.device,
                                   nano_fs.offset + nano_fs.page_size * file_info->page_index + start, af,
                                   data);
    }
    return 0;
}

int nano_fs_record_fetch(nano_fs_file_info_t *file_info, int type) {

    uint16_t start = nano_fs_get_addr(type);
    size_t size;
    if (type == NANO_FS_STAT_TIMES_ADDR) {
        size = NANO_FS_STAT_TIMES_MAX;
    } else if (type == NANO_FS_WRITE_TIMES_ADDR) {
        size = NANO_FS_WRITE_TIMES_MAX;
    } else {
        size = NANO_FS_READ_TIMES_MAX;
    }

    uint8_t st[32];
    nano_fs_native_read_bytes(nano_fs.device, nano_fs.offset + nano_fs.page_size * file_info->page_index +
                                              start, size, st);
    uint16_t times = 0;
    for (size_t s = 0; s < size; s++) {
        times = times + 8;
        int d = st[s];
        while (d > 0) {
            d = d >> 1;
            (times)--;
        }
    }
    return times;
}

nano_fs_ret
nano_fs_do_stat(const uint8_t *filename, uint16_t filename_len, nano_fs_file_info_t *file_info) {
    if (nano_fs.status == 0) {
        return NANO_FS_NOT_READY;
    }
    int i;
    //read storage page status,
    uint8_t page_status[NANO_FS_PAGE_STATUS_SIZE];
    uint8_t fnl[NANO_FS_FILENAME_LEN_SIZE];
    uint8_t name[NANO_FS_FILENAME_SIZE];
//    uint8_t creater[NANO_FS_CREATOR_SIZE];
//    uint8_t created[NANO_FS_CREATED_SIZE];
    int cn;
    for (i = 0; i < nano_fs.page_count; i++) {
        cn = nano_fs_native_read_bytes(nano_fs.device, nano_fs.offset + nano_fs.page_size * i +
                                                       nano_fs_get_addr(NANO_FS_PAGE_STATUS_ADDR),
                                       NANO_FS_PAGE_STATUS_SIZE, page_status);
        if (cn < NANO_FS_PAGE_STATUS_SIZE) {
            return NANO_FS_UNDEFINED_ERROR;
        }
        if (nano_fs_page_has_data(page_status[0])) {
            cn = nano_fs_native_read_bytes(nano_fs.device, nano_fs.offset + nano_fs.page_size * i +
                                                           nano_fs_get_addr(NANO_FS_FILENAME_OFFSET_ADDR),
                                           NANO_FS_FILENAME_LEN_SIZE, fnl);
            uint16_t _filename_len = nano_fs_len_func_hre(fnl, NANO_FS_FILENAME_LEN_SIZE);
            if (cn < NANO_FS_FILENAME_LEN_SIZE) {
                return NANO_FS_UNDEFINED_ERROR;
            }
            if (_filename_len >= 64 || _filename_len == 0) {
                continue;
            }
            cn = nano_fs_native_read_bytes(nano_fs.device, nano_fs.offset + nano_fs.page_size * i +
                                                           nano_fs_get_addr(NANO_FS_FILENAME_OFFSET_ADDR) +
                                                           NANO_FS_FILENAME_LEN_SIZE,
                                           _filename_len, name);
            if (cn < _filename_len) {
                return NANO_FS_UNDEFINED_ERROR;
            }
            if (filename_len == _filename_len && !(nano_fs_array_cmp(filename, name, filename_len))) {

                uint8_t content_offset[NANO_FS_CONTENT_OFFSET_SIZE];
                int _off = NANO_FS_CONTENT_OFFSET;
                while (1) {
                    nano_fs_native_read_bytes(nano_fs.device, nano_fs.offset + nano_fs.page_size * i +
                                                              _off,
                                              NANO_FS_CONTENT_OFFSET_SIZE, content_offset);
                    if (content_offset[0] == 0xFF && content_offset[1] == 0xFF) {
                        break;
                    } else {
                        _off = nano_fs_len_func_hre(content_offset, NANO_FS_CONTENT_OFFSET_SIZE);
                        if (_off > 0xFFFF) {
                            return NANO_FS_UNDEFINED_ERROR;
                        }
                    }
                }
                uint8_t content_len[NANO_FS_CONTENT_LEN_SIZE];
                nano_fs_native_read_bytes(nano_fs.device, nano_fs.offset + nano_fs.page_size * i +
                                                          _off + NANO_FS_CONTENT_OFFSET_SIZE,
                                          NANO_FS_CONTENT_LEN_SIZE, content_len);
                file_info->page_index = i;
                uint16_t stat_times = nano_fs_record_fetch(file_info, NANO_FS_STAT_TIMES_ADDR);
                uint16_t write_times = nano_fs_record_fetch(file_info, NANO_FS_WRITE_TIMES_ADDR);
                uint16_t read_times = nano_fs_record_fetch(file_info, NANO_FS_READ_TIMES_ADDR);

                file_info->content_len = nano_fs_len_func_hre(content_len, NANO_FS_CONTENT_LEN_SIZE);
                file_info->content_offset = _off;
                strcpy((char *) file_info->filename, (char *) filename);
                file_info->filename_len = filename_len;
                file_info->op_times = stat_times + write_times + read_times;
                file_info->stat_times = stat_times;
                file_info->read_times = read_times;
                file_info->write_times = write_times;
                return NANO_FS_NO_ERROR;
            }
        } else {
            continue;
        }
    }
    return NANO_FS_NOT_FOUND;
}

nano_fs_ret nano_fs_stat(const uint8_t *filename, uint16_t filename_len, nano_fs_file_info_t *file_info) {
    nano_fs_ret ret = nano_fs_do_stat(filename, filename_len, file_info);
    if (ret == NANO_FS_NO_ERROR) {
        file_info->stat_times = file_info->stat_times + 1;
        file_info->op_times = file_info->op_times + 1;
        nano_fs_record_op(file_info, NANO_FS_STAT_TIMES_ADDR, 1);
    }
    return ret;
}

int nano_fs_read_by_info(nano_fs_file_info_t *nanofs_file_info, uint8_t *buf) {
    return nano_fs_native_read_bytes(nano_fs.device, nano_fs.offset + nano_fs.page_size * nanofs_file_info->page_index +
                                                     nanofs_file_info->content_offset + NANO_FS_CONTENT_OFFSET_SIZE +
                                                     NANO_FS_CONTENT_LEN_SIZE,
                                     nanofs_file_info->content_len,
                                     buf);
}

int nano_fs_read(const uint8_t *filename, uint16_t filename_len, uint8_t *buf) {
    if (nano_fs.status == 0) {
        return NANO_FS_NOT_READY;
    }
    nano_fs_file_info_t nanofs_file_info;
    nano_fs_ret ret = nano_fs_do_stat(filename, filename_len, &nanofs_file_info);
    if (ret) {
        return ret;
    }
    nano_fs_record_op(&nanofs_file_info, NANO_FS_READ_TIMES_ADDR, 1);
    return nano_fs_read_by_info(&nanofs_file_info, buf);
}

int nano_fs_do_write(nano_fs_file_info_t file_info, const uint8_t *filename, uint16_t filename_l, uint8_t *buf,
                     uint16_t len, uint8_t is_new) {
    int index = file_info.page_index;
    uint8_t data[NANO_FS_PAGE_STATUS_SIZE];

    int o_content_offset = 0;
    int n_content_offset = NANO_FS_CONTENT_OFFSET;
    if (is_new) {
        nano_fs_native_erase(nano_fs.device, nano_fs.offset + nano_fs.page_size * index, nano_fs.page_size);
        nano_fs_record_op(&file_info, NANO_FS_WRITE_TIMES_ADDR, 1);
        o_content_offset = 0;
    } else if (!nano_fs.force_erase_before_write) {
        nano_fs_record_op(&file_info, NANO_FS_WRITE_TIMES_ADDR, 1);
        o_content_offset = 0;
    } else if (nano_fs.force_erase_before_write) {
        if (nano_fs.page_size - file_info.content_offset - file_info.content_len - NANO_FS_CONTENT_OFFSET_SIZE +
            NANO_FS_CONTENT_LEN_SIZE < len + NANO_FS_CONTENT_OFFSET_SIZE + NANO_FS_CONTENT_LEN_SIZE) {
            nano_fs_native_erase(nano_fs.device, nano_fs.offset + nano_fs.page_size * index, nano_fs.page_size);
            nano_fs_record_op(&file_info, NANO_FS_STAT_TIMES_ADDR, file_info.stat_times);
            nano_fs_record_op(&file_info, NANO_FS_READ_TIMES_ADDR, file_info.read_times);
            nano_fs_record_op(&file_info, NANO_FS_WRITE_TIMES_ADDR, file_info.write_times + 1);
            o_content_offset = 0;
        } else {
            nano_fs_record_op(&file_info, NANO_FS_WRITE_TIMES_ADDR, 1);
            o_content_offset = file_info.content_offset;
            n_content_offset = file_info.content_offset + NANO_FS_CONTENT_OFFSET_SIZE + NANO_FS_CONTENT_LEN_SIZE +
                               file_info.content_len;
        }
    }


    if (is_new || !nano_fs.force_erase_before_write || (nano_fs.force_erase_before_write &&
                                                        (nano_fs.page_size - file_info.content_offset -
                                                         file_info.content_len - 8 < len))) {
        data[0] = NANOFS_FILE_STATUS_WRITING;
        nano_fs_native_write_bytes(nano_fs.device, nano_fs.offset + nano_fs.page_size * index +
                                                   nano_fs_get_addr(NANO_FS_PAGE_STATUS_ADDR),
                                   NANO_FS_PAGE_STATUS_SIZE, data);
    }

    if (is_new || (nano_fs.force_erase_before_write &&
                   (nano_fs.page_size - file_info.content_offset - file_info.content_len < len))) {
        uint8_t filename_len[NANO_FS_FILENAME_LEN_SIZE];
        nano_fs_len_func_hel(filename_len, NANO_FS_FILENAME_LEN_SIZE, filename_l);
        nano_fs_native_write_bytes(nano_fs.device,
                                   nano_fs.offset + nano_fs.page_size * index +
                                   nano_fs_get_addr(NANO_FS_FILENAME_OFFSET_ADDR),
                                   NANO_FS_FILENAME_LEN_SIZE, filename_len);
        nano_fs_native_write_bytes(nano_fs.device,
                                   nano_fs.offset + nano_fs.page_size * index +
                                   nano_fs_get_addr(NANO_FS_FILENAME_OFFSET_ADDR) + NANO_FS_FILENAME_LEN_SIZE,
                                   filename_l, filename);
    }

    if (o_content_offset) {
        uint8_t content_offset[NANO_FS_CONTENT_OFFSET_SIZE];
        nano_fs_len_func_hel(content_offset, NANO_FS_CONTENT_OFFSET_SIZE, n_content_offset);
        nano_fs_native_write_bytes(nano_fs.device,
                                   nano_fs.offset + nano_fs.page_size * index + o_content_offset,
                                   NANO_FS_CONTENT_OFFSET_SIZE, content_offset);
    }

    uint8_t content_len[NANO_FS_CONTENT_LEN_SIZE];
    nano_fs_len_func_hel(content_len, NANO_FS_CONTENT_LEN_SIZE, len);
    nano_fs_native_write_bytes(nano_fs.device,
                               nano_fs.offset + nano_fs.page_size * index + n_content_offset +
                               NANO_FS_CONTENT_OFFSET_SIZE,
                               NANO_FS_CONTENT_LEN_SIZE, content_len);

    nano_fs_native_write_bytes(nano_fs.device,
                               nano_fs.offset + nano_fs.page_size * index + n_content_offset +
                               NANO_FS_CONTENT_LEN_SIZE + NANO_FS_CONTENT_LEN_SIZE,
                               len, buf);


    if (is_new || !nano_fs.force_erase_before_write) {
        data[0] = NANOFS_FILE_STATUS_OK;
        nano_fs_native_write_bytes(nano_fs.device, nano_fs.offset + nano_fs.page_size * index +
                                                   nano_fs_get_addr(NANO_FS_PAGE_STATUS_ADDR),
                                   NANO_FS_PAGE_STATUS_SIZE, data);
    }
    return len;
}

int nano_fs_write(const uint8_t *filename, uint16_t filename_len, uint8_t *buf, uint16_t len) {
    if (nano_fs.status == 0) {
        return NANO_FS_NOT_READY;
    }

    nano_fs_file_info_t file_info;
    int ret = nano_fs_do_stat(filename, filename_len, &file_info);
    if (ret == NANO_FS_NO_ERROR) {
        return nano_fs_do_write(file_info, filename, filename_len, buf, len, 0);
    } else if (ret == NANO_FS_NOT_FOUND) {
        int index;
        uint8_t data[NANO_FS_PAGE_STATUS_SIZE];
        for (index = 0; index < nano_fs.page_count; index++) {
            int cn;
            cn = nano_fs_native_read_bytes(nano_fs.device, nano_fs.offset + nano_fs.page_size * index +
                                                           nano_fs_get_addr(NANO_FS_PAGE_STATUS_ADDR),
                                           NANO_FS_PAGE_STATUS_SIZE, data);
            if (cn < NANO_FS_PAGE_STATUS_SIZE) {
                return NANO_FS_UNDEFINED_ERROR;
            }

            if (!nano_fs_page_has_data(data[0])) {
                file_info.page_index = index;
                file_info.stat_times = 0;
                file_info.write_times = 0;
                file_info.read_times = 0;
                file_info.content_offset = NANO_FS_CONTENT_OFFSET_SIZE;
                return nano_fs_do_write(file_info, filename, filename_len, buf, len, 1);
            }

        }
        return NANO_FS_INSUFFICIENT_SPACE;
    } else {
        return ret;
    }
}

int nano_fs_delete(const uint8_t *filename, uint16_t filename_len) {
    if (nano_fs.status == 0) {
        return NANO_FS_NOT_READY;
    }
    nano_fs_file_info_t file_info;
    int ret = nano_fs_do_stat(filename, filename_len, &file_info);

    if (ret == NANO_FS_NO_ERROR) {
        uint8_t data[] = {NANOFS_FILE_STATUS_DELETED};
        nano_fs_native_write_bytes(nano_fs.device, nano_fs.offset + nano_fs.page_size * file_info.page_index +
                                                   nano_fs_get_addr(NANO_FS_PAGE_STATUS_ADDR),
                                   NANO_FS_PAGE_STATUS_SIZE, data);
    } else {
        return ret;
    }
}

int nano_fs_array_cmp(const uint8_t *arr1, const uint8_t *arr2, uint16_t size) {
    for (uint16_t index = 0; index < size; index++) {
        if (arr1[index] != arr2[index])
            return 1;
    }
    return 0;
}

nano_fs_ret nano_fs_init(void *device, int offset, uint16_t page_size, uint8_t page_count,
                         uint8_t force_erase_before_write) {
    if (nano_fs.status == 1) {
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
    nano_fs.device = device;
    nano_fs.offset = offset;
    nano_fs.page_size = page_size;
    nano_fs.page_count = page_count;
    nano_fs.status = 1;
    nano_fs.force_erase_before_write = force_erase_before_write;
//    nano_fs.overwrite = overwrite;
    return NANO_FS_NO_ERROR;
}
