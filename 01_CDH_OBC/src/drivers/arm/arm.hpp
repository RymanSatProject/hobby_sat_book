/**
 * @file
 * @brief
 */

#ifndef _ARM_HPP__
#define _ARM_HPP__

#include "mbed.h"
#include "../drv8830/drv8830.hpp"

class Arm : public DRV8830 {
public:
    Arm(I2C* i2c, pinstat_t a0, pinstat_t a1, PinName pin_pos);
    ~Arm();

    uint8_t extend(void);
    uint8_t fold(void);
    uint8_t force_fold(uint16_t msec, uint32_t mv);
    uint8_t force_extend(uint16_t msec, uint32_t mv);

    /**
     * @brief Get potentiometer's value
     */
    uint16_t position(void);
    uint16_t position_avg(void);

private:
    AnalogIn* posmeter;
    Timer*    timer;

    uint8_t drive(uint8_t mode);
    uint8_t drive(uint8_t mode, uint16_t msec, uint32_t mv);

    bool is_position_abnormal(uint16_t pos);
};

#endif // _ARM_HPP__
