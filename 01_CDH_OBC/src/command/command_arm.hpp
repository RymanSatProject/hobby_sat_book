/**
 * @file
 * @brief
 */

#ifndef __COMMAND_ARM_PARAMS_HPP__
#define __COMMAND_ARM_PARAMS_HPP__

enum drive_arm_mode : uint8_t {
    DRIVE_ARM_MODE_EXTEND = 0x00,
    DRIVE_ARM_MODE_FOLD = 0x01,
    DRIVE_ARM_MODE_FORCE_EXTEND = 0x10,
    DRIVE_ARM_MODE_FORCE_FOLD = 0x11,
    DRIVE_ARM_MODE_TAKE_PIC = 0x20,
};

constexpr int SELFIE_DURATION_MARGIN_USEC = 5 * 1000 * 1000;

constexpr uint16_t RESP_LEN = 6;

#endif // __COMMAND_ARM_PARAMS_HPP__
