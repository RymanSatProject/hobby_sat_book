/**
 * @file
 * @brief
 */

#include "common/common.hpp"
#include "command.hpp"

#include "command_cam_config.hpp"

void Command::cmd_get_cam_config(Telecommand &cmd) {
    logger_info("[%s]\n", __func__);

    send_ack(cmd, RESP_OK);

    camcfg_t cfg;

    uint8_t commstat = dev->rpi->get_cam_config(&cfg);

    if (ack_on_rpi_failure(cmd, commstat)) {
        return;
    }

    uint8_t resp[RESP_GET_LEN];

    u16_to_bytes(cfg.iso, &resp[0], &resp[1]);

    uint8_t tmp = 0;
    u32_to_bytes(cfg.shutter, &tmp, &resp[2], &resp[3], &resp[4]);
    resp[5] = cfg.quality;
    resp[6] = (uint8_t)cfg.sharpness;
    resp[7] = (uint8_t)cfg.contrast;
    resp[8] = cfg.brightness;
    resp[9] = (uint8_t)cfg.saturation;
    resp[10] = cfg.exposure;

    Telemetry tlm(resp, RESP_GET_LEN);
    socket->send(cmd, tlm);
}

void Command::cmd_set_cam_config(Telecommand &cmd) {
    logger_info("[%s]\n", __func__);

    send_ack(cmd, RESP_OK);

    camcfg_t cfg;

    uint8_t *buf = cmd.data->buf;

    cfg.iso = to_u16(buf[0], buf[1]);
    cfg.shutter = to_u32(0, buf[2], buf[3], buf[4]);
    cfg.quality = buf[5];
    cfg.sharpness = static_cast<int8_t>(buf[6]);
    cfg.contrast = static_cast<int8_t>(buf[7]);
    cfg.brightness = buf[8];
    cfg.saturation = static_cast<int8_t>(buf[9]);
    cfg.exposure = buf[10];

    uint8_t commstat = dev->rpi->set_cam_config(&cfg);

    if (ack_on_rpi_failure(cmd, commstat)) {
        return;
    }

    uint8_t resp[RESP_SET_LEN] = {commstat};

    Telemetry tlm(resp, RESP_SET_LEN);
    socket->send(cmd, tlm);
}
