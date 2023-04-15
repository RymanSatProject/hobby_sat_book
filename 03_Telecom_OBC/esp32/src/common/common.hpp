#ifndef __COMMON_HPP__
#define __COMMON_HPP__

#include <Arduino.h>
#include "../lib/lock/lock.hpp"
#include "../lib/bytes.hpp"
#include "config.hpp"
#include "logger.hpp"

/**
 * @brief RF protocols available
 */
typedef enum : uint8_t {
    RFPROT_BT = 0,
    RFPROT_CW = 1,
} rfprot_t;

typedef struct data_s {
    uint8_t* buf;
    uint16_t size;
    Lock*    lock;
    rfprot_t prot; // used on host -> rf
} data_t;

/**
 * @brief Data bridge between host & RF
 * @param up: RF -> host
 * @param down: host -> RF
 */
typedef struct _bridge {
    data_t up;
    data_t down;
} bridge_t;

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

#endif // __COMMON_HPP__
