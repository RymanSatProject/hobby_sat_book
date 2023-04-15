/**
 * @file
 * @brief
 */

#include "common/logger.hpp"
#include "command.hpp"

#include "command_reset.hpp"

void Command::cmd_reset(Telecommand &cmd) {
    uint8_t target = cmd.data->buf[0];

    logger_info("[%s] target: 0x%02X\n", __func__, target);

    if ((target & TARGET_ALL) == 0) {
        send_ack(cmd, RESP_INVALID_ARGS);
        return;

    } else {
        send_ack(cmd, RESP_OK);
    }

    uint8_t resp[RESP_LEN] = {0};

    if (target & TARGET_COMM) {
        resp[0] = dev->comm->reset();
    }

    Telemetry tlm(resp, RESP_LEN);
    socket->send(cmd, tlm);

    if (target & TARGET_MAINOBC) {
        logger_info("[%s] System reset\n", __func__);
        NVIC_SystemReset();
    }
}
