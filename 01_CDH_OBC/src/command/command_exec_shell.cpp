/**
 * @file
 * @brief
 */

#include "common/logger.hpp"
#include "command.hpp"

#define CMDLEN_MAX 256

void Command::cmd_exec_shell(Telecommand &cmd) {
    logger_info("[%s]\n", __func__);

    send_ack(cmd, RESP_OK);

    char     input[CMDLEN_MAX + 1] = "";
    uint16_t cmdlen;

    if (cmd.data->size > CMDLEN_MAX) {
        logger_warning("[%s] command too long\n");
        cmdlen = CMDLEN_MAX;

    } else {
        cmdlen = cmd.data->size;
    }

    memcpy(input, cmd.data->buf, cmdlen);

    char msg[256];

    uint8_t commstat = dev->rpi->exec_shell(input, 256, msg);

    if (ack_on_rpi_failure(cmd, commstat)) {
        return;
    }

    uint8_t resp[256];

    strcpy((char *)resp, msg);
    uint16_t size = strlen(msg) + 1;

    Telemetry tlm(resp, size);
    socket->send(cmd, tlm);
}
