/**
 * @file
 * @brief
 */

#ifndef __CONFIG_STM32F401RE_HPP__
#define __CONFIG_STM32F401RE_HPP__

#include "common/types.hpp"

constexpr int      BAUD_RPI = 115200;
constexpr int      BAUD_COMM = 115200;
constexpr float    VREF = 3.3;
constexpr uint32_t HKDATA_SIZE = 60;
constexpr int      WATCHDOG_TIMEOUT_MS = 30000;

// I2C1
constexpr PinName PIN_I2C_SCL = PB_8;
constexpr PinName PIN_I2C_SDA = PB_9;

constexpr PinName PIN_TEMP = PA_0;
constexpr PinName PIN_POW_RPI = PA_4;

// UART2
constexpr PinName PIN_RPI_TX = PA_9;
constexpr PinName PIN_RPI_RX = PA_10;

// UART6
constexpr PinName PIN_COMM_TX = PC_6;
constexpr PinName PIN_COMM_RX = PC_7;

constexpr PinName PIN_COMM_INT = PC_11;

constexpr PinName PIN_ARM = PA_1;

constexpr uint8_t ARM_PINSTAT_A0 = PINSTAT_LOW;
constexpr uint8_t ARM_PINSTAT_A1 = PINSTAT_HIZ;
constexpr uint8_t RW_PINSTAT_A0 = PINSTAT_HIZ;
constexpr uint8_t RW_PINSTAT_A1 = PINSTAT_HIZ;

#endif // __CONFIG_STM32F401RE_HPP__
