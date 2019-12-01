/*  Nano Fs internal define
 *
 * */

typedef enum {
    NANOFS_FILE_STATUS_EMPTY = 0xFF,
    NANOFS_FILE_STATUS_WRITING = 0xFE,
    NANOFS_FILE_STATUS_UPDATING = 0xFc,
    NANOFS_FILE_STATUS_OK = 0xF8,
    NANOFS_FILE_STATUS_ERROR = 0xF0,
    NANOFS_FILE_STATUS_DELETED = 0x00,
} nanofs_file_status;

typedef struct {
    char filename[64];
    uint8_t filename_len;
    uint16_t file_len;
    char creator[64];
    uint8_t creator_len;
    uint64_t created;
    uint16_t optimes;
} nanofs_file_inner_info_t;

typedef struct {
    int offset;
    uint16_t page_size;
    uint8_t page_count;
    uint8_t status; // 1 for ready,0 for not ready
    uint8_t overwrite;
    uint8_t erase_before_write;
    void *device;
} nanofs_t;