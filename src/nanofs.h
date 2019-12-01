#ifndef NANO_FS_H
#define NANO_FS_H

#include <stdint.h>
#include <stdio.h>

#define NANO_FS_ID_STR "OSFS"
#define NANO__VER 1

#define NANOFS_OPTIMES_ENABLED

#define  NANOFS_PAGE_STATUS_SIZE 1  //page status  0x00:file is ok,  0x01:file is writing,  0x02:file is udpating,  0xFF:empty page,ready to write new file
#define  NANOFS_FILENAME_LEN_SIZE 1 //file name length storage len
#define  NANOFS_CONTENT_OFFSET_SIZE 2 //file content offset storage len
#define  NANOFS_CONTENT_LEN_SIZE 2 //file content len storage len
#define  NANOFS_CREATED_SIZE 8 // uint64 date

#ifndef  NANOFS_PAGE_SIZE_UNIT
#define NANOFS_PAGE_SIZE_UNIT 1024 //page size should be times of NANOFS_PAGE_SIZE_UNIT
#endif

#ifndef  NANOFS_CONTENT_OFFSET
#define NANOFS_CONTENT_OFFSET 1024 //storage left for file info
#endif

#ifndef  NANOFS_FILENAME_SIZE
#define  NANOFS_FILENAME_SIZE 64 //filename len
#endif

#ifndef  NANOFS_CREATOR_SIZE
#define  NANOFS_CREATOR_SIZE 64 //file creator info len
#endif

#ifdef  NANOFS_OPTIMES_ENABLED
#ifndef NANOFS_OPTIMES_MAX
#define  NANOFS_OPTIMES_MAX 4 //4 bytes,Max 32 times
#endif
#endif

#if  NANOFS_OPTIMES_MAX
#ifndef NANOFS_STAT_TIMES_MAX
#define  NANOFS_STAT_TIMES_MAX NANOFS_OPTIMES_MAX/2 //2 bytes,Max 16 times
#endif
#ifndef NANOFS_WRITE_TIMES_MAX
#define  NANOFS_WRITE_TIMES_MAX NANOFS_OPTIMES_MAX/2 //2 bytes,Max 16 times
#endif
#ifndef NANOFS_READ_TIMES_MAX
#define  NANOFS_READ_TIMES_MAX NANOFS_OPTIMES_MAX //4 bytes,Max 32 times
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern int nanofs_native_read_bytes(void *device, uint32_t address, uint16_t size, uint8_t *output);

extern int nanofs_native_write_bytes(void *device, uint32_t address, uint16_t size, const uint8_t *input);

extern int nanofs_native_erase(void *device, uint32_t address, uint16_t size);

typedef struct {
    char filename[64];
    uint8_t filename_len;
    uint16_t file_len;
    char creator[64];
    uint8_t creator_len;
    uint64_t created;
    uint16_t optimes;

    //TODO move to inner
    uint8_t page_index;
    uint16_t content_offset;
} nanofs_file_info_t;

typedef enum {
    NANOFS_NO_ERROR = 0,
    NANOFS_WRONG_VERSION = -1,
    NANOFS_UNCAUGHT_OOR = -2,
    NANOFS_NOT_FOUND = -3,
    NANOFS_INSUFFICIENT_SPACE = -4,
    NANOFS_UNFORMATTED = -5,
    NANOFS_BUFFER_WRONG_SIZE = -6,
    NANOFS_FILE_ALREADY_EXISTS = -7,
    NANOFS_UNDEFINED_ERROR = -8,
    NANOFS_INITED = -9,
    NANOFS_INIT_INVALID = -10,
    NANOFS_INIT_HEADER_INSUFFICIENT = -11,
    NANOFS_NOT_READY = -12,
} nanofs_ret;

nanofs_ret nanofs_stat(const char *filename, nanofs_file_info_t *file_info);

int nanofs_read(const char *filename, uint8_t *buf);

int nanofs_read_by_info(nanofs_file_info_t *nanofs_file_info, uint8_t *buf);

int nanofs_write(const char *filename, uint8_t *buf, uint16_t len);

int nanofs_delete(const char *filename);

nanofs_ret nanofs_init(void *device, int offset, uint16_t page_size, uint8_t page_count, uint8_t overwrite,
                       uint8_t erase_before_write);

#ifdef __cplusplus
}
#endif

#endif