#ifndef __I2C_DEVICE_HPP__
#define __I2C_DEVICE_HPP__

#include "mbed.h"
#include "drivers/device/device.hpp"

/**
 * @brief Device with I2C bus
 *
 */
class I2CDevice : public Device {
public:
    I2CDevice();
    ~I2CDevice();
    bool is_online(void);
    static void scan(I2C* i2c);

protected:
    I2C*    i2c;
    uint8_t i2caddr;
    bool online;
};

#endif // __I2C_DEVICE_HPP__
