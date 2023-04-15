/**
 * @file
 * @brief
 */

#ifndef __COMMAND_RWHEEL_HPP__
#define __COMMAND_RWHEEL_HPP__

enum cmd_rwheel_mode : uint8_t {
    DRIVE_RWHEEL_MODE_START = 0,
    DRIVE_RWHEEL_MODE_STOP = 1,
    DRIVE_RWHEEL_MODE_MAX = 1,
};

constexpr uint16_t RESP_LEN = 1;

#endif // __COMMAND_RWHEEL_HPP__
