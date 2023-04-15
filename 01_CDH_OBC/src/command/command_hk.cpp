/**
 * @file
 * @brief
 */

#include "common/logger.hpp"
#include "command.hpp"
#include "drivers/commdev/commdev.hpp"

static uint8_t buf[Commdev::mtu];

void Command::cmd_get_hkdata(Telecommand &cmd) {
    uint8_t offset = cmd.data->buf[0];
    uint8_t reqnum = cmd.data->buf[1];

    logger_info("[%s] offset = %u reqnum = %u\n", __func__, offset, reqnum);

    if (reqnum > hk->size) {
        logger_warning("[%s] reqnum too large: %d\n", __func__, reqnum);
        reqnum = hk->size;
    }

    if (reqnum == 0 || offset > hk->size - 1) {
        send_ack(cmd, RESP_INVALID_ARGS);
        return;

    } else {
        send_ack(cmd, RESP_OK);
    }

    uint16_t unitnum = socket->maxsize() / hk->unitsize;
    uint16_t buflen = unitnum * hk->unitsize;
    bool     first = true;
    bool     aligned = reqnum % buflen == 0;
    uint16_t chunknum = reqnum / unitnum;
    uint16_t restnum = reqnum % unitnum;
    logger_debug("[%s] configs: %u %u %d %d %u %u\n", __func__, buflen, unitnum,
                 first, aligned, chunknum, restnum);

    size_t   i = 0;
    uint16_t seq = 0;

    for (i = 0; i < chunknum; i++) {
        for (size_t k = 0; k < unitnum; k++) {
            hk->serialize(
                &buf[k * hk->unitsize],
                (hk->idx - (i * unitnum + k + offset) + 2 * hk->size) %
                    hk->size);
        }

        Telemetry tlm(buf, buflen);
        socket->send(cmd, tlm, aligned && i == chunknum, first, seq);
        wait_us(100000);

        first = false;
        seq++;
    }

    if (restnum == 0) {
        return;
    }

    for (size_t k = 0; k < restnum; k++) {
        hk->serialize(
            &buf[k * hk->unitsize],
            (hk->idx - (i * unitnum + k + offset) + hk->size) % hk->size);
    }

    Telemetry tlm(buf, restnum * hk->unitsize);
    socket->send(cmd, tlm, true, first, seq);
}
