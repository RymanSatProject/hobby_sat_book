/**
 * @file
 * @brief
 */

#include "common/common.hpp"
#include "command.hpp"
#include "command_get_pic_list.hpp"

void Command::cmd_get_pic_list(Telecommand &cmd) {
    uint8_t *buf = cmd.data->buf;
    uint16_t start = to_u16(buf[0], buf[1]);
    uint16_t end = to_u16(buf[2], buf[3]);

    logger_info("[%s] start = %u, end = %u\n", __func__, start, end);

    uint8_t ack = (start > end) ? RESP_INVALID_ARGS : RESP_OK;

    send_ack(cmd, ack);

    if (ack != RESP_OK) {
        return;
    }

    uint32_t piclist[219]; // TODO: why 219?
    uint16_t count = 0;
    uint8_t  result = 0;

    uint8_t commstat =
        dev->rpi->get_pic_list(start, end, piclist, count, result);

    if (ack_on_rpi_failure(cmd, commstat)) {
        return;
    }

    uint8_t resp[MAINOBC_PIC_LIST_MAXNUM * 4 + 2];

    resp[0] = result;
    resp[1] = count & 0xFF;

    // 上位2byteがID、下位2byteがsubID
    for (uint16_t i = 0; i < count; i++) {
        if (i >= MAINOBC_PIC_LIST_MAXNUM) {
            break;
        }

        // バイトオーダー入れ替え
        resp[2 + i * 4] = static_cast<uint8_t>((piclist[i] >> 24) & 0xFF);
        resp[2 + i * 4 + 1] = static_cast<uint8_t>((piclist[i] >> 16) & 0xFF);
        resp[2 + i * 4 + 2] = static_cast<uint8_t>((piclist[i] >> 8) & 0xFF);
        resp[2 + i * 4 + 3] = static_cast<uint8_t>((piclist[i]) & 0xFF);
    }

    uint16_t size = 4 * count + 2;

    Telemetry tlm(resp, size);
    socket->send(cmd, tlm);
}
