/**
 * @file
 * @brief
 */

#include "crc.hpp"
#include <stdio.h>
#include <string.h>

int16_t crc8(uint8_t str[], uint16_t len) {
    uint8_t crc8 = 0;

    for (uint16_t i = 0; i < len; i++) {
        crc8 ^= str[i];

        for (uint8_t j = 0; j < 8; j++) {
            if (crc8 & 0x80) {
                crc8 <<= 1;
                crc8 ^= 0x07;
            } else
                crc8 <<= 1;
        }
    }
    return crc8;
}

bool is_crc8_valid(int16_t crc, uint8_t str[], uint16_t len) {
    return crc8(str, len) == crc;
}
