/**
 * @file
 * @brief
 */

#include "common/common.hpp"
#include "command/command_param.hpp"
#include "maindev/maindev.hpp"
#include "net/net.hpp"

#ifndef __CMDPARSE_FUNC_HPP__
#define __CMDPARSE_FUNC_HPP__

int        cmdparse_test(Telecommand &cmd, Array<char *> &arg);
const char cmd_test_comm_help[] = "Usage: test-comm <n>\n";

int        cmdparse_debug(Telecommand &cmd, Array<char *> &arg);
const char cmd_debug_help[] = "Usage: debug <n>\n";

int        cmdparse_kill_rpi(Telecommand &cmd, Array<char *> &arg);
const char cmd_kill_rpi_help[] = "Usage: kill-rpi";

int        cmdparse_help(Telecommand &cmd, Array<char *> &arg);
const char cmd_help_help[] = "Usage: help";

#endif // __CMDPARSE_FUNC_HPP__
