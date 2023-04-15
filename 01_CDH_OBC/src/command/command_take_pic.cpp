/**
 * @file
 * @brief
 */

#include "common/common.hpp"
#include "command.hpp"
#include "command_take_pic.hpp"

void Command::cmd_take_pic(Telecommand &cmd) {
    uint8_t *buf = cmd.data->buf;

    uint16_t interval_ms = to_u16(buf[0], buf[1]);
    uint16_t duration_ms = to_u16(buf[2], buf[3]);

    logger_info("[%s] interval = %u, duration = %u\n", __func__, interval_ms,
                duration_ms);

    send_ack(cmd, RESP_OK);

    uint8_t resp[RESP_LEN];
    uint8_t commstat = dev->rpi->take_pic(interval_ms, duration_ms, 0, resp);

    if (ack_on_rpi_failure(cmd, commstat)) {
        return;
    }

    Telemetry tlm(resp, RESP_LEN);
    socket->send(cmd, tlm);
}
