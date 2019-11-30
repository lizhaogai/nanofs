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
    int offset;
    uint16_t page_size;
    uint8_t page_count;
    uint8_t status; // 1 for ready,0 for not ready
    uint8_t overwrite;
    void *device;
} nanofs_t;