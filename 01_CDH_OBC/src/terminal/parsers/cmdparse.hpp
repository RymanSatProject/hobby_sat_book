/**
 * @file
 * @brief
 */

#include "common/common.hpp"
#include "cmdparse_func.hpp"

#ifndef __CMDPARSE_HPP__
#define __CMDPARSE_HPP__

typedef struct {
    const char *name;
    uint8_t     cmd;
    int (*parse)(Telecommand &cmd, Array<char *> &arg);
    const char *help;
} parserlist_t;

const parserlist_t parserlist[] = {
    {"test-comm", CMD_TEST_COMM, cmdparse_test, cmd_test_comm_help},
    {"debug", CMD_DEBUG, cmdparse_debug, cmd_debug_help},
    {"kill-rpi", CMD_KILL_RPI, cmdparse_kill_rpi, cmd_kill_rpi_help},
    {"help", CMD_HELP, cmdparse_help, cmd_help_help},
};

#endif // __CMDPARSE_HPP__
