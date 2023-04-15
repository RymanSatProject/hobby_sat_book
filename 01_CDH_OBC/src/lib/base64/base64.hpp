/**
 * @file
 * @brief
 */

#ifndef __BASE64_HPP__
#define __BASE64_HPP__

#include "mbed.h"

uint8_t base64_to_6bit(uint8_t c);

uint16_t base64_decode(char *src, char *dst, uint16_t srclen);

void base64_encode(char *src, char *dst, uint16_t len, uint16_t *dstlen);

#endif // __BASE64_HPP__
