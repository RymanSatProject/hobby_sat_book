/**
 * @file Network-layer protocol
 * @brief
 */

#ifndef __SPP_HPP__
#define __SPP_HPP__

#include "mbed.h"

typedef struct packet_ {
    uint8_t  reserved : 8;
    uint8_t  command : 8;
    uint8_t  last : 1;
    uint8_t  first : 1;
    uint16_t seq : 14;
    uint16_t size : 16;
    uint32_t id : 32;
    uint8_t *payload;
} packet_t;

class Packet {
public:
    static packet_t create(uint8_t command, bool last, bool first, uint16_t seq,
                           uint16_t size, uint32_t id, uint8_t *payload);
    static packet_t create_empty(void);

    static int encode(packet_t &pkt, uint8_t buf[], uint16_t &buflen);
    static int decode(packet_t &pkt, uint8_t buf[], uint16_t buflen);

    static const uint16_t headlen = 10;
};

#endif // __SPP_HPP__
