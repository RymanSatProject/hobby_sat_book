/**
 * @file
 * @brief
 */

#include "common/common.hpp"
#include "command.hpp"

#include "command_arm.hpp"

void Command::cmd_drive_arm(Telecommand &cmd) {
    uint8_t *buf = cmd.data->buf;
    uint8_t  mode = buf[0];
    uint8_t  resp[RESP_LEN];
    Arm *    arm = dev->arm;

    memset(resp, 0, RESP_LEN);

    if (mode != DRIVE_ARM_MODE_EXTEND && mode != DRIVE_ARM_MODE_FOLD &&
        mode != DRIVE_ARM_MODE_FORCE_EXTEND &&
        mode != DRIVE_ARM_MODE_FORCE_FOLD && mode != DRIVE_ARM_MODE_TAKE_PIC) {
        send_ack(cmd, RESP_INVALID_ARGS);
        return;

    } else {
        send_ack(cmd, RESP_OK);
    }

    uint16_t duration_arm_ms = 3000;
    uint32_t mv = 3000;

    switch (mode) {
        case DRIVE_ARM_MODE_EXTEND:
            logger_debug("%s: auto-extend\n", __func__);
            resp[0] = arm->extend();
            break;

        case DRIVE_ARM_MODE_FOLD:
            logger_debug("%s: auto-fold\n", __func__);
            resp[1] = arm->fold();
            break;

        case DRIVE_ARM_MODE_FORCE_EXTEND: {
            logger_debug("%s: force extend\n", __func__);
            resp[0] = arm->force_extend(duration_arm_ms, mv);
            break;
        }

        case DRIVE_ARM_MODE_FORCE_FOLD: {
            logger_debug("%s: force fold\n", __func__);
            resp[1] = arm->force_fold(duration_arm_ms, mv);
            break;
        }

        case DRIVE_ARM_MODE_TAKE_PIC: {
            logger_debug("%s: extend & take pic & fold\n", __func__);

            uint8_t rpi_status = dev->rpi->comm_check();

            if (rpi_status != RPI_COMM_SUCCESS) {
                logger_error("%s: RPi not active\n", __func__);
                resp[4] = rpi_status;
                break;
            }

            resp[0] = arm->extend();
            resp[4] = dev->rpi->take_pic(0, 0, 0, &resp[5]);
            wait_us(SELFIE_DURATION_MARGIN_USEC);
            resp[1] = arm->fold();
        }

        default: {
            break;
        }
    }

    uint16_t pos = arm->position();
    logger_info("%s: Arm position = %d\n", __func__, pos);
    resp[2] = (pos >> 8) & 0xFF;
    resp[3] = pos & 0xFF;

    Telemetry tlm(resp, RESP_LEN);
    socket->send(cmd, tlm);
}
