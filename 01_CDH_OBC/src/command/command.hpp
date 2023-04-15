/**
 * @file
 * @brief
 */

#ifndef __COMMAND_HPP__
#define __COMMAND_HPP__

#include "common/common.hpp"
#include "command_param.hpp"
#include "maindev/maindev.hpp"
#include "system/hkdata.hpp"
#include "power/power.hpp"
#include "net/net.hpp"

class Command {
public:
    Command(MainDevice *dev, HKdata *hk, Power *power, SpaceSocket *socket);
    ~Command();

    void exec(Telecommand &cmd);

    void cmd_test_comm(Telecommand &cmd);
    void cmd_debug(Telecommand &cmd);
    void cmd_get_hkdata(Telecommand &cmd);
    void cmd_reset(Telecommand &cmd);
    void cmd_power(Telecommand &cmd);

    void cmd_drive_arm(Telecommand &cmd);

    void cmd_take_pic(Telecommand &cmd);
    void cmd_get_pic_list(Telecommand &cmd);
    void cmd_get_pic_info(Telecommand &cmd);
    void cmd_get_pic_data(Telecommand &cmd);
    void cmd_delete_pic(Telecommand &cmd);
    void cmd_set_pic_size(Telecommand &cmd);
    void cmd_set_comp_pic(Telecommand &cmd);
    void cmd_kill_rpi(Telecommand &cmd);

    void cmd_set_cam_config(Telecommand &cmd);
    void cmd_get_cam_config(Telecommand &cmd);
    void cmd_start_pic_categ(Telecommand &cmd);
    void cmd_check_pic_categ(Telecommand &cmd);
    void cmd_abort_pic_categ(Telecommand &cmd);
    void cmd_get_pic_categ(Telecommand &cmd);
    void cmd_get_good_pic_ids(Telecommand &cmd);

    void cmd_exec_shell(Telecommand &cmd);
    void cmd_uplink_file(Telecommand &cmd);
    void cmd_downlink_file(Telecommand &cmd);

    void cmd_drive_rwheel(Telecommand &cmd);
    void cmd_config_rwheel(Telecommand &cmd);

    void split_downlink(Telecommand &cmd, uint8_t data[], const uint16_t size,
                        uint8_t header);

private:
    MainDevice * dev;
    HKdata *     hk;
    Power *      power;
    SpaceSocket *socket;

    uint8_t *rpibuf;

    /**
     * @brief Downlink ACK telemetry
     * @param resp: ACK or NACK
     */
    void send_ack(Telecommand &cmd, uint8_t resp);

    bool ack_on_rpi_failure(Telecommand &cmd, uint8_t commstat);
};

#endif // __COMMAND_HPP__
