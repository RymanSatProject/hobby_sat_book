/**
 * @file
 * @brief
 */

#include "common/logger.hpp"
#include "command.hpp"

#include "command_kill_rpi.hpp"

void Command::cmd_kill_rpi(Telecommand &cmd) {
    logger_info("[%s]\n", __func__);

    send_ack(cmd, RESP_OK);

    uint8_t resp[RESP_LEN];
    uint8_t commstat = dev->rpi->kill(resp);

    if (ack_on_rpi_failure(cmd, commstat)) {
        return;
    }

    Telemetry tlm(resp, RESP_LEN);
    socket->send(cmd, tlm);
}
