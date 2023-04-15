/**
 * @file
 * @brief
 */

#include "protocol.hpp"
#include "common/common.hpp"

constexpr uint8_t  RESERVED = 0x0;
constexpr uint16_t SEQ_MASK = 0x3FFF;
constexpr uint8_t  LS_BIT = BIT(7);
constexpr uint8_t  FS_BIT = BIT(6);
constexpr uint16_t HEADER_LEN = 10;

packet_t Packet::create(uint8_t command, bool last, bool first, uint16_t seq,
                        uint16_t size, uint32_t id, uint8_t *payload) {
    return {RESERVED, command, last, first, seq, size, id, payload};
}

packet_t Packet::create_empty(void) {
    return {RESERVED, 0, 0, 0, 0, 0, 0, nullptr};
}

/**
 * @brief encode a packet into a binary string
 * @return 0 on success, others on failure
 */
int Packet::encode(packet_t &pkt, uint8_t buf[], uint16_t &buflen) {
    buf[0] = RESERVED;
    buf[1] = pkt.command;

    u16_to_bytes(pkt.seq & SEQ_MASK, buf + 2, buf + 3);

    if (pkt.last) {
        buf[2] |= LS_BIT;
    }

    if (pkt.first) {
        buf[2] |= FS_BIT;
    }

    u16_to_bytes(pkt.size, buf + 4, buf + 5);
    u32_to_bytes(pkt.id, buf + 6, buf + 7, buf + 8, buf + 9);

    memcpy(buf + 10, pkt.payload, pkt.size);
    buflen = HEADER_LEN + pkt.size;

    return 0;
}

/**
 * @brief decode a given binary string into a packet
 * @param pkt: must have payload[] that is large enough to store incoming
 * data
 * @return 0 on success, others on failure
 */
int Packet::decode(packet_t &pkt, uint8_t buf[], uint16_t buflen) {
    packet_t pkt_tmp;

    pkt_tmp.reserved = buf[0];
    pkt_tmp.command = buf[1];
    pkt_tmp.last = (buf[2] & LS_BIT) > 0;
    pkt_tmp.first = (buf[2] & FS_BIT) > 0;
    pkt_tmp.seq = to_u16(buf[2], buf[3]) & SEQ_MASK;
    pkt_tmp.size = to_u16(buf[4], buf[5]);
    pkt_tmp.id = to_u32(buf[6], buf[7], buf[8], buf[9]);
    pkt_tmp.payload = buf + 10;

    if (pkt_tmp.reserved != RESERVED) {
        logger_error("%s: Invalid reserved: %u\n", __func__, pkt_tmp.reserved);
        return -EINVAL;
    }

    if (pkt_tmp.size > buflen - HEADER_LEN) {
        logger_error("%s: pkt_tmp.size too large: %u > %u\n", __func__,
                     pkt_tmp.size, buflen);
        return -EINVAL;
    }

    pkt.reserved = pkt_tmp.reserved;
    pkt.command = pkt_tmp.command;
    pkt.last = pkt_tmp.last;
    pkt.first = pkt_tmp.first;
    pkt.seq = pkt_tmp.seq;
    pkt.size = pkt_tmp.size;
    pkt.id = pkt_tmp.id;
    memcpy(pkt.payload, pkt_tmp.payload, pkt_tmp.size);

    logger_debug("%s: cmd=%x last=%u first=%u seq=%u size=%u id=%u payload=\n",
                 __func__, pkt.command, pkt.last, pkt.first, pkt.seq, pkt.size,
                 pkt.id);
    logger_hexdump(VERB_DEBUG, pkt.payload, pkt.size);

    return 0;
}
