#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void randomData(uint8_t *data, uint16_t size) {
    for (int i = 0; i < size; i++) {
        data[i] = rand() & 0xff;
    }
}