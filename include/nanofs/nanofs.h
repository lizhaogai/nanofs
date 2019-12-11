#ifndef NANO_FS_H
#define NANO_FS_H

#include <stdint.h>
#include <stdio.h>

#define NANO_FS_ID_STR "OSFS"
#define NANO__VER 1

#define NANO_FS_OPTIMES_ENABLED

#define  NANO_FS_PAGE_STATUS_SIZE 1  //page status  0x00:file is ok,  0x01:file is writing,  0x02:file is udpating,  0xFF:empty page,ready to write new file
#define  NANO_FS_FILENAME_LEN_SIZE 1 //file name length storage len
#define  NANO_FS_CONTENT_OFFSET_SIZE 2 //file content offset storage len
#define  NANO_FS_CONTENT_LEN_SIZE 2 //file content len storage len
#define  NANO_FS_CREATED_SIZE 8 // uint64 date

#ifndef  NANO_FS_PAGE_SIZE_UNIT
#define NANO_FS_PAGE_SIZE_UNIT 1024 //page size should be times of NANOFS_PAGE_SIZE_UNIT
#endif

#ifndef  NANO_FS_CONTENT_OFFSET
#define NANO_FS_CONTENT_OFFSET 1024 //storage left for file info
#endif

#ifndef  NANO_FS_FILENAME_SIZE
#define  NANO_FS_FILENAME_SIZE 64 //filename len
#endif

#ifndef  NANO_FS_CREATOR_SIZE
#define  NANO_FS_CREATOR_SIZE 64 //file creator info len
#endif

#ifdef  NANO_FS_OPTIMES_ENABLED
#ifndef NANO_FS_OPTIMES_MAX
#define  NANO_FS_OPTIMES_MAX 4 //4 bytes,Max 32 times
#endif
#endif

#if  NANO_FS_OPTIMES_MAX
#ifndef NANO_FS_STAT_TIMES_MAX
#define  NANO_FS_STAT_TIMES_MAX NANO_FS_OPTIMES_MAX/2 //2 bytes,Max 16 times
#endif
#ifndef NANO_FS_WRITE_TIMES_MAX
#define  NANO_FS_WRITE_TIMES_MAX NANO_FS_OPTIMES_MAX/2 //2 bytes,Max 16 times
#endif
#ifndef NANO_FS_READ_TIMES_MAX
#define  NANO_FS_READ_TIMES_MAX NANO_FS_OPTIMES_MAX //4 bytes,Max 32 times
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern int nano_fs_native_read_bytes(void *device, uint32_t address, uint16_t size, uint8_t *output);

extern int nano_fs_native_write_bytes(void *device, uint32_t address, uint16_t size, const uint8_t *input);

extern int nano_fs_native_erase(void *device, uint32_t address, uint16_t size);

typedef struct {
    char filename[64];
    uint8_t filename_len;
    uint16_t content_len;
    char creator[64];
    uint8_t creator_len;
    uint64_t created;
    uint16_t optimes;

    //TODO move to inner
    uint8_t page_index;
    uint16_t content_offset;
} nano_fs_file_info_t;

typedef enum {
    NANO_FS_NO_ERROR = 0,
    NANO_FS_WRONG_VERSION = -1,
    NANO_FS_UNCAUGHT_OOR = -2,
    NANO_FS_NOT_FOUND = -3,
    NANO_FS_INSUFFICIENT_SPACE = -4,
    NANO_FS_UNFORMATTED = -5,
    NANO_FS_BUFFER_WRONG_SIZE = -6,
    NANO_FS_FILE_ALREADY_EXISTS = -7,
    NANO_FS_UNDEFINED_ERROR = -8,
    NANO_FS_INITED = -9,
    NANO_FS_INIT_INVALID = -10,
    NANO_FS_INIT_HEADER_INSUFFICIENT = -11,
    NANO_FS_NOT_READY = -12,
} nano_fs_ret;

nano_fs_ret nano_fs_stat(const uint8_t *filename, uint16_t filename_len, nano_fs_file_info_t *file_info);

int nano_fs_read(const uint8_t *filename, uint16_t filename_len, uint8_t *buf);

int nano_fs_read_by_info(nano_fs_file_info_t *nanofs_file_info, uint8_t *buf);

int nano_fs_write(const uint8_t *filename, uint16_t filename_len, uint8_t *buf, uint16_t len);

int nano_fs_delete(const uint8_t *filename, uint16_t filename_len);

nano_fs_ret nano_fs_init(void *device, int offset, uint16_t page_size, uint8_t page_count, uint8_t overwrite,
                         uint8_t erase_before_write);

int nano_fs_array_cmp(const uint8_t *arr1, const uint8_t *arr2, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif