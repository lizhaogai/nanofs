#include <string.h>
#include "header.h"

uint8_t storage[512 * 1024];

void fake_storage_init() {
    memset(storage, 0xFF, sizeof(storage));
}

int nano_fs_native_read_bytes(void *device, uint32_t address, uint16_t size, uint8_t *output) {
    int index;
    for (index = 0; index < size; index++) {
        output[index] = storage[index + address];
    }
    return size;
}

int nano_fs_native_write_bytes(void *device, uint32_t address, uint16_t size, const uint8_t *input) {
    int index;
    for (index = 0; index < size; index++) {
        storage[index + address] = input[index];
    }
    return 0;
}

int nano_fs_native_erase(void *device, uint32_t address, uint16_t size) {
    int index;
    for (index = 0; index < size; index++) {
        storage[index + address] = 0xFF;
    }
    return 0;
}