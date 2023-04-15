/**
 * @file
 * @brief
 */

#ifndef __TERMINAL_HPP__
#define __TERMINAL_HPP__

#include "mbed.h"
#include "common/common.hpp"
#include "system/system.hpp"

class Terminal {
public:
    Terminal(Serial *serial, MainSystem *sys);
    ~Terminal();

    void run(void);

private:
    Serial *    serial;
    MainSystem *sys;

    Array<uint8_t> *input;
    Array<char *> * arg;
    bool            full;

    /**
     * @brief Parse input into a telecommand
     * @return 0 on success, others on failure
     */
    int find_command(Telecommand &cmd);

    /**
     * @brief Get incoming message
     */
    void get_input(void);
    void clear_input(void);

    int  set_payload(Telecommand &cmd, Array<char *> &arg);
    void print_head(void);
};

#endif // __TERMINAL_HPP__
