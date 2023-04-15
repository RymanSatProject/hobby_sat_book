/**
 * @file
 * @brief
 */

#include "common/common.hpp"
#include "command/command_param.hpp"
#include "maindev/maindev.hpp"

#include "cmdparse.hpp"
#include "command/command_test.hpp"

int cmdparse_debug(Telecommand &cmd, Array<char *> &arg) {
    cmd.data->buf[0] = arg[1] ? atoi(arg[1]) & 0xFF : 0;
    cmd.data->size = 1;

    return 0;
}
