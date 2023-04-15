/**
 * @file
 * @brief
 */

#include "command.hpp"
#include "common/common.hpp"

#include "command_delete_pic.hpp"

void Command::cmd_delete_pic(Telecommand &cmd) {
    uint8_t *buf = cmd.data->buf;
    uint16_t id = to_u16(buf[0], buf[1]);
    uint16_t sub_id = to_u16(buf[2], buf[3]);
    uint8_t  mode = buf[4];

    logger_info("[%s] id = %u, sub_id = %u mode = %u\n", __func__, id, sub_id,
                mode);

    uint8_t ack = (mode > 2) ? RESP_INVALID_ARGS : RESP_OK;

    send_ack(cmd, ack);

    if (ack != RESP_OK) {
        return;
    }

    uint8_t result;
    uint8_t commstat = dev->rpi->delete_pic(id, sub_id, mode, &result);

    if (ack_on_rpi_failure(cmd, commstat)) {
        return;
    }

    uint8_t resp[RESP_LEN] = {result};

    Telemetry tlm(resp, RESP_LEN);
    socket->send(cmd, tlm);
}
