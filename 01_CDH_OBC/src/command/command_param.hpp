/**
 * @file
 * @brief
 */

#ifndef __COMMAND_PARAM_HPP__
#define __COMMAND_PARAM_HPP__

// システム
constexpr uint8_t CMD_NONE = 0x00;
constexpr uint8_t CMD_TEST_COMM = 0x01;
constexpr uint8_t CMD_DEBUG = 0x05;
constexpr uint8_t CMD_BEACON = 0x10;
constexpr uint8_t CMD_GET_HKDATA = 0x11;
constexpr uint8_t CMD_RESET = 0x12;
constexpr uint8_t CMD_POWER = 0x20;

// アーム
constexpr uint8_t CMD_DRIVE_ARM = 0x31;

// RPi: カメラ
constexpr uint8_t CMD_TAKE_PIC = 0x33;
constexpr uint8_t CMD_GET_PIC_LIST = 0x34;
constexpr uint8_t CMD_GET_PIC_INFO = 0x35;
constexpr uint8_t CMD_GET_PIC_DATA = 0x36;
constexpr uint8_t CMD_DELETE_PIC = 0x38;
constexpr uint8_t CMD_SET_PIC_SIZE = 0x39;
constexpr uint8_t CMD_KILL_RPI = 0x3B;
constexpr uint8_t CMD_GET_CAM_CONFIG = 0x3C;
constexpr uint8_t CMD_SET_CAM_CONFIG = 0x3D;

// RPi: 画像認識
constexpr uint8_t CMD_START_PIC_CATEG = 0x41;
constexpr uint8_t CMD_CHECK_PIC_CATEG = 0x42;
constexpr uint8_t CMD_ABORT_PIC_CATEG = 0x43;
constexpr uint8_t CMD_GET_PIC_CATEG = 0x44;
constexpr uint8_t CMD_GET_GOOD_PIC_IDS = 0x45;

// RPi: その他
constexpr uint8_t CMD_EXEC_SHELL = 0x56;
constexpr uint8_t CMD_UPLINK_FILE = 0x3E;
constexpr uint8_t CMD_DOWNLINK_FILE = 0x3F;

// 姿勢制御
constexpr uint8_t CMD_DRIVE_RWHEEL = 0x60;
constexpr uint8_t CMD_CONFIG_RWHEEL = 0x61;

constexpr uint8_t CMD_HELP = 0xFF;

// その他
constexpr uint8_t CMD_ACK = 0xE0;

constexpr uint8_t RESP_OK = 0x00;
constexpr uint8_t RESP_NG = 0xF0;
constexpr uint8_t RESP_INVALID_ARGS = 1u << 0;
constexpr uint8_t RESP_INVALID_CMD = 1u << 1;
constexpr uint8_t RESP_INVALID_CRC = 1u << 2;
constexpr uint8_t RESP_TARGET_OFF = 1u << 3;

constexpr uint8_t RESP_RPI_POS = 4;

#endif // __COMMAND_PARAM_HPP__
