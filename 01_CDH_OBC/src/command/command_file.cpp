/**
 * @file
 * @brief
 */

#include "common/common.hpp"
#include "command.hpp"
#include "lib/base64/base64.hpp"
#include "command_file.hpp"

void Command::cmd_uplink_file(Telecommand &cmd) {
    logger_info("[%s]\n", __func__);

    send_ack(cmd, RESP_OK);

    char *buf = (char *)cmd.data->buf;

    uint32_t pos = to_u32(0, buf[0], buf[1], buf[2]);
    uint16_t size = to_u16(buf[3], buf[4]);

    char filename[FILENAME_LEN + 1] = "";

    memcpy(filename, &buf[5], FILENAME_LEN);

    char data[DATA_LEN] = "";
    memcpy(data, &buf[5 + FILENAME_LEN], size);

    char     base64_data[DATA_BASE64_LEN] = "";
    uint16_t base64_size;
    base64_encode((char *)data, base64_data, size, &base64_size);

    uint8_t commstat =
        dev->rpi->uplink_file(filename, pos, base64_size, base64_data);

    if (ack_on_rpi_failure(cmd, commstat)) {
        return;
    }

    uint8_t resp[RESP_LEN] = {commstat};

    Telemetry tlm(resp, RESP_LEN);
    socket->send(cmd, tlm);
}

void Command::cmd_downlink_file(Telecommand &cmd) {
    logger_info("[%s]\n", __func__);

    send_ack(cmd, RESP_OK);

    char *buf = (char *)cmd.data->buf;

    uint32_t pos = to_u32(0, buf[0], buf[1], buf[2]);
    uint16_t size = to_u16(buf[3], buf[4]);
    uint16_t filepath_size = to_u16(buf[5], buf[6]);

    char *filepath = new char[filepath_size + 1];
    memset((char *)filepath, 0, filepath_size + 1);
    strncpy(filepath, (const char *)&buf[7], filepath_size);

    uint16_t buflen;
    uint8_t  commstat =
        dev->rpi->downlink_file(filepath, pos, size, rpibuf, &buflen);

    delete filepath;

    if (ack_on_rpi_failure(cmd, commstat)) {
        return;
    }

    split_downlink(cmd, rpibuf, buflen, commstat);
}
