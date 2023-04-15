#include "mbed.h"

#ifndef __ERRNO_HPP__
#define __ERRNO_HPP__

constexpr uint8_t errno_to_u8(int err) {
    return (-err) & 0xFF;
}

#endif // __ERRNO_HPP__
