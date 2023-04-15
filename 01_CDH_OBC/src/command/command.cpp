/**
 * @file
 * @brief
 */

#include "command.hpp"
#include "net/net.hpp"

uint8_t payload[COMM_BUF_SIZE - 1];
uint8_t _rpibuf[RPIBUF_LEN];

Command::Command(MainDevice *dev, HKdata *hk, Power *power, SpaceSocket *socket) {
    this->dev = dev;
    this->hk = hk;
    this->power = power;
    this->socket = socket;

    rpibuf = _rpibuf;
}

Command::~Command() {
}

void Command::exec(Telecommand &cmd) {
    switch (cmd.cmd) {
        case CMD_TEST_COMM:
            cmd_test_comm(cmd);
            break;

        case CMD_DEBUG:
            cmd_debug(cmd);
            break;

        case CMD_GET_HKDATA:
            cmd_get_hkdata(cmd);
            break;

        case CMD_RESET:
            cmd_reset(cmd);
            break;

        case CMD_POWER:
            cmd_power(cmd);
            break;

        case CMD_DRIVE_ARM:
            cmd_drive_arm(cmd);
            break;

        case CMD_TAKE_PIC:
            cmd_take_pic(cmd);
            break;

        case CMD_GET_PIC_LIST:
            cmd_get_pic_list(cmd);
            break;

        case CMD_GET_PIC_INFO:
            cmd_get_pic_info(cmd);
            break;

        case CMD_GET_PIC_DATA:
            cmd_get_pic_data(cmd);
            break;

        case CMD_DELETE_PIC:
            cmd_delete_pic(cmd);
            break;

        case CMD_SET_PIC_SIZE:
            cmd_set_pic_size(cmd);
            break;

        case CMD_KILL_RPI:
            cmd_kill_rpi(cmd);
            break;

        case CMD_SET_CAM_CONFIG:
            cmd_set_cam_config(cmd);
            break;

        case CMD_GET_CAM_CONFIG:
            cmd_get_cam_config(cmd);
            break;

        case CMD_START_PIC_CATEG:
            cmd_start_pic_categ(cmd);
            break;

        case CMD_CHECK_PIC_CATEG:
            cmd_check_pic_categ(cmd);
            break;

        case CMD_ABORT_PIC_CATEG:
            cmd_abort_pic_categ(cmd);
            break;

        case CMD_GET_PIC_CATEG:
            cmd_get_pic_categ(cmd);
            break;

        case CMD_GET_GOOD_PIC_IDS:
            cmd_get_good_pic_ids(cmd);
            break;

        case CMD_EXEC_SHELL:
            cmd_exec_shell(cmd);
            break;

        case CMD_UPLINK_FILE:
            cmd_uplink_file(cmd);
            break;

        case CMD_DOWNLINK_FILE:
            cmd_downlink_file(cmd);
            break;

        case CMD_DRIVE_RWHEEL:
            cmd_drive_rwheel(cmd);
            break;

        case CMD_CONFIG_RWHEEL:
            cmd_config_rwheel(cmd);
            break;

        default:
            logger_error("%s: command not found: %02X\n", __func__, cmd.cmd);
    }
}

void Command::send_ack(Telecommand &cmd, uint8_t resp) {
    uint8_t     buf[2] = {cmd.cmd, resp};
    Telemetry   tlm(buf, ARRAY_SIZE(buf));
    Telecommand cmd_ack(CMD_ACK, cmd.id, 0);

    logger_info("%s: cmd=%u resp=%u\n", __func__, cmd.cmd, resp);
    socket->send(cmd_ack, tlm);
}

bool Command::ack_on_rpi_failure(Telecommand &cmd, uint8_t commstat) {
    if (commstat == RPI_COMM_SUCCESS) {
        return false; // i.e. failure ACK is not sent

    } else {
        logger_error("[%s] comm failure\n", __func__);
        send_ack(cmd, (commstat << RESP_RPI_POS) & 0xFF);
        return true;
    }
}

constexpr uint16_t MAX_SIZE_OF_DATA = 255;

void Command::split_downlink(Telecommand &cmd, uint8_t data[],
                             const uint16_t size, uint8_t header) {
    uint8_t resp[MAX_SIZE_OF_DATA + 1];

    resp[0] = header;
    uint16_t seq = 0;

    for (uint16_t i = 0; i < size; i += MAX_SIZE_OF_DATA) {
        logger_debug("[%s] From byte %u:\n", __func__, i);

        uint8_t datlen =
            (size - i > MAX_SIZE_OF_DATA) ? MAX_SIZE_OF_DATA : size - i;
        uint8_t resp_len = datlen + 1;

        memcpy(&resp[1], &data[i], datlen);

        Telemetry tlm(resp, resp_len);
        socket->send(cmd, tlm, i == size - 1, i == 0, seq);
        wait_us(100000);

        logger_debug("[%s] datlen = %u\n", __func__, datlen);
        logger_debug("[%s] Data: ", __func__);
        logger_hexdump(VERB_DEBUG, resp, datlen);

        seq++;
    }
}
