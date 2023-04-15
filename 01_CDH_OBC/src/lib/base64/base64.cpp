/**
 * @file
 * @brief
 */

#include "base64.hpp"
#include "src/common/types.hpp"

uint8_t base64_to_6bit(uint8_t c) {
    if (c == '=') return 0;
    if (c == '/') return 63;
    if (c == '+') return 62;
    if (c <= '9') return (c - '0') + 52;
    if ('a' <= c) return (c - 'a') + 26;
    return (c - 'A');
}

uint16_t base64_decode(char *src, char *dst, uint16_t srclen) {
    char *p = dst;

    for (uint16_t n = 0; n < srclen; n++) {
        if (src[n] == '=') {
            break;
        }

        uint8_t val = base64_to_6bit(src[n]);

        switch (n % 4) {
            case 0:
                *p = (val << 2) & 0xfc;
                break;

            case 1:
                *(p++) |= (val >> 4) & 0x03;
                *p = (val << 4) & 0xf0;
                break;

            case 2:
                *(p++) |= (val >> 2) & 0x0f;
                *p = (val << 6) & 0xc0;
                break;

            default:
                *(p++) |= val & 0x3f;
        }
    }

    return static_cast<uint16_t>(p - dst);
}

void base64_encode(char *src, char *dst, uint16_t len, uint16_t *dstlen) {
    const char table[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    uint8_t  mod = len % 3;
    uint16_t adj_len = len - mod;
    char    *p = dst;
    uint8_t  o0, o1, o2, o3;
    uint8_t  mask[3] = {0b11111100, 0b11110000, 0b11000000};

    for (int16_t i = 0; i < adj_len; i += 3) {
        o0 = (src[i] & mask[0]) >> 2;
        o1 = ((src[i] & ~mask[0]) << 4) | ((src[i + 1] & mask[1]) >> 4);
        o2 = ((src[i + 1] & ~mask[1]) << 2) | ((src[i + 2] & mask[2]) >> 6);
        o3 = src[i + 2] & ~mask[2];
        *p++ = table[o0];
        *p++ = table[o1];
        *p++ = table[o2];
        *p++ = table[o3];
    }

    // Encode residue
    if (mod) {
        if (mod == 1) {
            o0 = (src[adj_len] & mask[0]) >> 2;
            o1 = ((src[adj_len] & ~mask[0]) << 4) |
                 ((src[adj_len + 1] & mask[1]) >> 4);
            *p++ = table[o0];
            *p++ = table[o1];
            *p++ = '=';
            *p++ = '=';
        } else if (mod == 2) {
            o0 = (src[adj_len] & mask[0]) >> 2;
            o1 = ((src[adj_len] & ~mask[0]) << 4) |
                 ((src[adj_len + 1] & mask[1]) >> 4);
            o2 = ((src[adj_len + 1] & ~mask[1]) << 2) |
                 ((src[adj_len + 2] & mask[2]) >> 6);
            *p++ = table[o0];
            *p++ = table[o1];
            *p++ = table[o2];
            *p++ = '=';
        }
    }
    *p = 0;

    *dstlen = int(p - dst);
}
