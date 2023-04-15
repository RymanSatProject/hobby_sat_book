/**
 * @file
 * @brief
 */

#include "common/common.hpp"
#include "command.hpp"
#include "command_get_pic_info.hpp"

void Command::cmd_get_pic_info(Telecommand &cmd) {
    uint8_t *buf = cmd.data->buf;
    uint16_t start = to_u16(buf[0], buf[1]);
    uint16_t end = to_u16(buf[2], buf[3]);
    uint8_t  type = buf[4];
    logger_info("[%s] start = %u, end = %u type = %d\n", __func__, start, end,
                type);

    uint8_t ack = (type > 1) ? RESP_INVALID_ARGS : RESP_OK;

    send_ack(cmd, ack);

    if (ack != RESP_OK) {
        return;
    }

    uint16_t count = 0;
    uint8_t  result = 0;

    // 現状、ラズパイとのバッファがファイル23個分の情報しかない
    // メッセージ分割を後回しにするためひとまず上限20とする
    PicInfo info[GET_PIC_INFO_MAXNUM]; // 13byte * 19 = 247byte

    uint8_t commstat =
        dev->rpi->get_pic_info(start, end, type, info, count, result);

    if (ack_on_rpi_failure(cmd, commstat)) {
        return;
    }

    if (count > GET_PIC_INFO_MAXNUM) {
        logger_error("%s: Too many picinfo: %d\n", count);
        count = GET_PIC_INFO_MAXNUM;
    }

    uint8_t resp[PICINFO_LEN * GET_PIC_INFO_MAXNUM + 2];

    resp[0] = result;
    resp[1] = count & 0xFF;

    for (uint16_t i = 0; i < count; i++) {
        // TODO ラズパイからの送信量を増やす場合は
        // ラズパイとの通信分割、地上へのパケット分割・繰り返し送信が必要
        u16_to_bytes((uint16_t)info[i].id, &resp[IDX_START + i * PICINFO_LEN],
                     &resp[IDX_START + i * PICINFO_LEN + 1]);
        u16_to_bytes((uint16_t)info[i].sub_id,
                     &resp[IDX_START + i * PICINFO_LEN + 2],
                     &resp[IDX_START + i * PICINFO_LEN + 3]);
        resp[IDX_START + i * PICINFO_LEN + 4] = info[i].type;
        u16_to_bytes((uint16_t)info[i].width,
                     &resp[IDX_START + i * PICINFO_LEN + 5],
                     &resp[IDX_START + i * PICINFO_LEN + 6]);
        u16_to_bytes((uint16_t)info[i].height,
                     &resp[IDX_START + i * PICINFO_LEN + 7],
                     &resp[IDX_START + i * PICINFO_LEN + 8]);
        uint8_t tmp;
        u32_to_bytes(info[i].size, &tmp, &resp[IDX_START + i * PICINFO_LEN + 9],
                     &resp[IDX_START + i * PICINFO_LEN + 10],
                     &resp[IDX_START + i * PICINFO_LEN + 11]);
        resp[IDX_START + i * PICINFO_LEN + 12] = info[i].categ;
    }

    uint16_t size = PICINFO_LEN * count + 2;

    Telemetry tlm(resp, size);
    socket->send(cmd, tlm);
}
