/**
 * @file
 * @brief
 */

#include "common/logger.hpp"
#include "command.hpp"

#include "command_reset.hpp"

void Command::cmd_power(Telecommand &cmd) {
    uint8_t target = cmd.data->buf[0];
    uint8_t state = cmd.data->buf[1];

    logger_info("[%s] %02X %02X\n", __func__, target, state);

    if (target < DEVID_NUM && state <= POWSTAT_ON) {
        send_ack(cmd, RESP_OK);

    } else {
        send_ack(cmd, RESP_INVALID_ARGS);
        return;
    }

    uint8_t resp[RESP_LEN] = {0};

    devid_t id = (devid_t)target;
    switch (id) {
        case DEVID_RPI:
            if (state == POWSTAT_ON) {
                resp[0] = errno_to_u8(dev->rpi->wakeup());
            } else if (state == POWSTAT_OFF) {
                resp[0] = errno_to_u8(dev->rpi->shutdown());
            } else {
                logger_error("[%s] Invalid state: 0x%02X\n", __func__, id);
            }

            break;

        default:
            logger_error("[%s] Invalid target: 0x%02X\n", __func__, target);
    }

    Telemetry tlm(resp, RESP_LEN);
    socket->send(cmd, tlm);
}
