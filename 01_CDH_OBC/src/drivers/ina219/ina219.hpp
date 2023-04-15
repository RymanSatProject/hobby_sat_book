/**
 * @file
 * @brief
 */

#ifndef _INA219_HPP_
#define _INA219_HPP_

#include "drivers/i2c/i2c_device.hpp"

class INA219 : public I2CDevice {
public:
    INA219(I2C *i2c);
    ~INA219();

    int init(void);
    void reset(void);

    float shunt_vol_mv(void);
    float bus_vol_mv(void);
    float current_ma(void);
    float power_mw(void);

private:
    uint32_t calreg_val; // value written to CALIBRATION reg
    float    coef_cur;
    float    coef_pow;

    int read(uint8_t addr_reg, uint16_t &data);
    int write(uint8_t addr_reg, uint16_t val);
};

#endif // _INA219_HPP_
