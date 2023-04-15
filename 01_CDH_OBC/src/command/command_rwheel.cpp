/**
 * @file
 * @brief
 */

#include "common/common.hpp"
#include "command.hpp"
#include "command_rwheel.hpp"

void Command::cmd_drive_rwheel(Telecommand &cmd) {
    uint8_t *buf = cmd.data->buf;

    uint8_t  mode = buf[0];
    int16_t  target = static_cast<int16_t>(to_u16(buf[1], buf[2]));
    uint32_t sec = to_u32(0, buf[3], buf[4], buf[5]);

    if (mode > DRIVE_RWHEEL_MODE_MAX) {
        logger_error("%s: Invalid mode: %u\n", __func__, mode);
        send_ack(cmd, RESP_INVALID_ARGS);
        return;
    }

    logger_debug("%s: mode = %u, target = %d, sec = %u\n", __func__, mode,
                 target, sec);

    send_ack(cmd, RESP_OK);


    switch (mode) {
        case DRIVE_RWHEEL_MODE_START:
            dev->rw->control(3000, DRV8830::MODE_FORWARD);
            wait_us(sec * 1000 * 1000);
            dev->rw->stop();
            break;

        case DRIVE_RWHEEL_MODE_STOP:
            dev->rw->stop();
            break;

        default:
            break;
    }

    dev->rw->clear_fault();

    uint8_t resp[RESP_LEN] = {0};

    Telemetry tlm(resp, RESP_LEN);
    socket->send(cmd, tlm);
}

void Command::cmd_config_rwheel(Telecommand &cmd) {
    // To be added in the future!

    uint8_t resp[1] = {0};

    Telemetry tlm(resp, 1);
    socket->send(cmd, tlm);
}
