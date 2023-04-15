/**
 * @file
 * @brief
 */

#include "common/logger.hpp"
#include "command.hpp"

#include "command_test.hpp"

void Command::cmd_test_comm(Telecommand &cmd) {
    uint8_t target = cmd.data->buf[0];
    if (target != TARGET_MAINOBC && target != TARGET_RPI) {
        logger_error("%s: invalid argument: %u\n", __func__, target);

        send_ack(cmd, RESP_INVALID_ARGS);
        return;

    } else {
        send_ack(cmd, RESP_OK);
    }

    logger_info("[%s] target = %s\n", __func__,
                target == TARGET_MAINOBC ? "main OBC" : "RPi");

    uint8_t resp;

    switch (target) {
        case TARGET_MAINOBC:
            resp = RESP_OK;
            break;

        case TARGET_RPI:
            resp =
                dev->rpi->comm_check() == RPI_COMM_SUCCESS ? RESP_OK : RESP_NG;
            break;

        default:
            logger_error("[%s] Invalid target\n");
    }

    logger_debug("[%s] %s\n", __func__,
                 resp == RESP_OK ? "success" : "failure");

    Telemetry tlm(&resp, 1);
    socket->send(cmd, tlm);
}
