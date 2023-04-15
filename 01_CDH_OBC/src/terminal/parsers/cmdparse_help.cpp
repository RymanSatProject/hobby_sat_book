/**
 * @file
 * @brief
 */

#include "common/common.hpp"
#include "command/command_param.hpp"
#include "maindev/maindev.hpp"

#include "cmdparse.hpp"

int cmdparse_help(Telecommand &cmd, Array<char *> &arg) {
    logger_base("Available commands:\n");
    for (size_t i = 0; i < ARRAY_SIZE(parserlist); i++) {
        logger_base("  ");
        logger_base("%s \n", parserlist[i].name);
    }

    logger_base("Add -h option to show each usage\n");

    return 1;
}
