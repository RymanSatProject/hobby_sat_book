/**
 * @file
 * @brief
 */

#ifndef __BYTES_HPP__
#define __BYTES_HPP__

#include <Arduino.h>

constexpr uint16_t to_u16(uint8_t high, uint8_t low) {
    return high << 8 | low;
}

constexpr uint32_t to_u32(uint8_t high, uint8_t midhigh, uint8_t midlow,
                          uint8_t low) {
    return high << 24 | midhigh << 16 | midlow << 8 | low;
}

inline void u16_to_bytes(uint16_t val, uint8_t* high, uint8_t* low) {
    *low = val & 0xFF;
    *high = (val >> 8) & 0xFF;
}

inline void u32_to_bytes(uint32_t val, uint8_t* high, uint8_t* midhigh,
                         uint8_t* midlow, uint8_t* low) {
    *low = val & 0xFF;
    *midlow = (val >> 8) & 0xFF;
    *midhigh = (val >> 16) & 0xFF;
    *high = (val >> 24) & 0xFF;
}

#endif // __BYTES_HPP__
