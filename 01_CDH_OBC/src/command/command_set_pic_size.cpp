/**
 * @file
 * @brief
 */

#include "common/common.hpp"
#include "command.hpp"
#include "command_set_pic_size.hpp"

void Command::cmd_set_pic_size(Telecommand &cmd) {
    uint8_t *buf = cmd.data->buf;
    uint8_t  type = buf[0];
    uint16_t width = to_u16(buf[1], buf[2]);
    uint16_t height = to_u16(buf[3], buf[4]);

    logger_info("[%s]\n", __func__);

    uint8_t ack = (type == PIC_SIZE_NORMAL || type == PIC_SIZE_COMP)
                      ? RESP_OK
                      : RESP_INVALID_ARGS;

    send_ack(cmd, ack);

    if (ack != RESP_OK) {
        return;
    }

    uint8_t result = 0;

    uint8_t commstat;

    switch (type) {
        case PIC_SIZE_NORMAL:
            commstat = dev->rpi->set_pic_size(width, height, &result);
            break;

        case PIC_SIZE_COMP:
            commstat = dev->rpi->set_pic_compression(width, height, &result);
            break;

        default:
            logger_error("[%s] Invalid type: %u\n", __func__, type);
            commstat = RPI_COMM_FAILURE;
    }

    if (ack_on_rpi_failure(cmd, commstat)) {
        return;
    }

    uint8_t resp[RESP_LEN] = {result};

    Telemetry tlm(resp, RESP_LEN);
    socket->send(cmd, tlm);
}
