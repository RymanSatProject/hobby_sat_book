/**
 * @file
 * @brief
 */

#include "common/common.hpp"
#include "command.hpp"
#include "command_pic_categ.hpp"

void Command::cmd_start_pic_categ(Telecommand &cmd) {
    uint8_t setting = cmd.data->buf[0];
    uint8_t work_type = (setting >> 6) & 0x01;
    uint8_t is_all = (setting >> 4) & 0x01;
    uint8_t do_wait = (setting >> 2) & 0x01;

    logger_info("[%s] work_type = %u, is_all = %u do_wait = %u\n", __func__,
                work_type, is_all, do_wait);

    send_ack(cmd, RESP_OK);

    uint8_t exe_result, cmd_result;
    uint8_t commstat = dev->rpi->start_pic_categ(work_type, is_all, do_wait,
                                                 &exe_result, &cmd_result);

    if (ack_on_rpi_failure(cmd, commstat)) {
        return;
    }

    uint8_t resp[RESP_START_LEN] = {exe_result, cmd_result};

    Telemetry tlm(resp, RESP_START_LEN);
    socket->send(cmd, tlm);
}

void Command::cmd_check_pic_categ(Telecommand &cmd) {
    logger_info("[%s]\n", __func__);

    send_ack(cmd, RESP_OK);

    uint8_t exe_result, cmd_result;
    uint8_t commstat = dev->rpi->check_pic_categ(&exe_result, &cmd_result);

    if (ack_on_rpi_failure(cmd, commstat)) {
        return;
    }

    uint8_t resp[RESP_CHECK_LEN] = {exe_result, cmd_result};

    Telemetry tlm(resp, RESP_CHECK_LEN);
    socket->send(cmd, tlm);
}

void Command::cmd_abort_pic_categ(Telecommand &cmd) {
    logger_info("[%s]\n", __func__);

    send_ack(cmd, RESP_OK);

    uint8_t exe_result, cmd_result;
    uint8_t commstat = dev->rpi->abort_pic_categ(&exe_result, &cmd_result);

    if (ack_on_rpi_failure(cmd, commstat)) {
        return;
    }

    uint8_t resp[RESP_ABORT_LEN] = {exe_result, cmd_result};

    Telemetry tlm(resp, RESP_ABORT_LEN);
    socket->send(cmd, tlm);
}

void Command::cmd_get_pic_categ(Telecommand &cmd) {
    uint8_t work_type = cmd.data->buf[0];

    logger_info("[%s]\n", __func__);

    uint8_t ack = (work_type == WORK_TYPE_TF || work_type == WORK_TYPE_TFL ||
                   work_type == WORK_TYPE_NO)
                      ? RESP_OK
                      : RESP_INVALID_ARGS;

    send_ack(cmd, ack);

    uint8_t *buf = cmd.data->buf;
    uint16_t id = to_u16(buf[1], buf[2]);
    uint16_t sub_id = to_u16(buf[3], buf[4]);
    uint8_t  exe_result, cmd_result, categ, percent;

    uint8_t commstat = dev->rpi->get_pic_categ(
        work_type, id, sub_id, &exe_result, &cmd_result, &categ, &percent);

    if (ack_on_rpi_failure(cmd, commstat)) {
        return;
    }

    uint8_t resp[RESP_GET_LEN] = {exe_result, cmd_result, categ, percent};

    Telemetry tlm(resp, RESP_GET_LEN);
    socket->send(cmd, tlm);
}

void Command::cmd_get_good_pic_ids(Telecommand &cmd) {
    logger_info("[%s]\n", __func__);

    send_ack(cmd, RESP_OK);

    uint16_t ids[NUM_GOODPIC_MAX];
    uint16_t sub_ids[NUM_GOODPIC_MAX];
    uint8_t  exe_result, cmd_result;
    uint8_t  count = 0;

    uint8_t commstat = dev->rpi->get_good_pic_ids(&exe_result, &cmd_result,
                                                  &count, ids, sub_ids);

    if (ack_on_rpi_failure(cmd, commstat)) {
        return;
    }

    uint8_t  resp[3 + NUM_GOODPIC_MAX * 4];
    uint16_t resp_len = 3 + count * 4;

    resp[0] = exe_result;
    resp[1] = cmd_result;
    resp[2] = count;

    for (uint8_t i = 0; i < count; i++) {
        u16_to_bytes(ids[i], &resp[3 + 4 * i], &resp[4 + 4 * i]);
        u16_to_bytes(sub_ids[i], &resp[5 + 4 * i], &resp[6 + 4 * i]);
    }

    Telemetry tlm(resp, resp_len);
    socket->send(cmd, tlm);
}
