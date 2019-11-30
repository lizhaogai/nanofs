#include <string.h>

uint8_t storage[512 * 1024];

void buffer_storage_init() {
    memset(storage, 0xFF, sizeof(storage));
}

int nanofs_native_read_bytes(void *device, uint32_t address, uint16_t size, uint8_t *output) {
    printf("nanofs_native_read_bytes\n");
    int index;
    for (index = 0; index < size; index++) {
        output[index] = storage[index + address];
    }
    return size;
}

int nanofs_native_write_bytes(void *device, uint32_t address, uint16_t size, const uint8_t *input) {
    printf("nanofs_native_write_bytes\n");
    return 0;
}

int nanofs_native_erase(void *device, uint32_t address, uint16_t size) {
    printf("nanofs_native_erase\n");
    return 0;
}