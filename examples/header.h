#ifndef NANO_FS_TEST_HEADER_H
#define NANO_FS_TEST_HEADER_H

#include <stdio.h>
#include <stdint.h>

void fake_storage_init();

int nano_fs_native_read_bytes(void *device, uint32_t address, uint16_t size, uint8_t *output);

int nano_fs_native_write_bytes(void *device, uint32_t address, uint16_t size, const uint8_t *input);

int nano_fs_native_erase(void *device, uint32_t address, uint16_t size);

int test_normal();

void randomData(uint8_t *data, uint16_t size);

int test_not_force_erase();

int test_force_erase();

#endif