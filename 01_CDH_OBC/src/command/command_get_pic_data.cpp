/**
 * @file
 * @brief
 */

#include "common/common.hpp"
#include "command.hpp"

#include "command_get_pic_data.hpp"

void Command::cmd_get_pic_data(Telecommand &cmd) {
    uint8_t *buf = cmd.data->buf;
    uint8_t  type = buf[0];
    uint16_t id = to_u16(buf[1], buf[2]);
    uint16_t sub_id = to_u16(buf[3], buf[4]);
    uint32_t pos = to_u32(0, buf[5], buf[6], buf[7]);
    uint32_t size = to_u32(0, buf[8], buf[9], buf[10]);

    uint8_t ack = (type == PIC_ORIGINAL || type == PIC_THUMBNAIL)
                      ? RESP_OK
                      : RESP_INVALID_ARGS;

    send_ack(cmd, ack);

    if (ack != RESP_OK) {
        return;
    }

    logger_info("[%s] id = %u, sub_id = %u pos = %u size = %u\n", __func__, id,
                sub_id, pos, size);

    const uint16_t headlen = 9;
    uint16_t       unitsize = socket->maxsize() - headlen;
    size_t         section = size / unitsize + 1;
    int            size_remaining = size;

    uint16_t resp_size = socket->maxsize();
    uint8_t *resp = new uint8_t[resp_size];
    bool     first = true;
    bool     last = false;
    uint16_t seq = 0;

    for (size_t i = 0; i < section; i++) {
        uint32_t _pos = pos + unitsize * i;
        uint16_t buflen;
        uint16_t _size = size_remaining < unitsize ? size_remaining : unitsize;

        uint8_t commstat = dev->rpi->get_pic_data(type, id, sub_id, _pos, _size,
                                                  rpibuf, &buflen);

        if (ack_on_rpi_failure(cmd, commstat)) {
            break;
        }

        logger_debug("%s: seq %u: got %u bytes\n", __func__, i, buflen);
        // logger_hexdump(VERB_DEBUG, (uint8_t *)rpibuf, buflen);

        uint16_t size_act = _size < buflen ? _size : buflen;

        uint8_t tmp;
        resp[0] = commstat;
        resp[1] = type;
        u16_to_bytes(id, &resp[2], &resp[3]);
        u16_to_bytes(sub_id, &resp[4], &resp[5]);
        u32_to_bytes(_pos, &tmp, &resp[6], &resp[7], &resp[8]);
        memcpy(resp + 9, rpibuf, size_act);

        size_remaining -= size_act;

        if (size_remaining <= 0 || buflen == 0) {
            last = true;
        }

        Telemetry tlm(resp, headlen + size_act);
        socket->send(cmd, tlm, last, first, seq);

        logger_debug("%s: seq %u end: remaining = %d act = %u\n", __func__, i,
                     size_remaining, size_act);

        if (last) {
            break;
        }

        seq++;
        first = false;
    }

    delete resp;
}
