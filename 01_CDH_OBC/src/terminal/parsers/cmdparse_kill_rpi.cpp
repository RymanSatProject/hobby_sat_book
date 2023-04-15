/**
 * @file
 * @brief
 */

#include "common/common.hpp"
#include "command/command_param.hpp"
#include "maindev/maindev.hpp"

#include "cmdparse.hpp"
#include "command/command_kill_rpi.hpp"

int cmdparse_kill_rpi(Telecommand &cmd, Array<char *> &arg) {
    cmd.data->size = 0;
    return 0;
}
