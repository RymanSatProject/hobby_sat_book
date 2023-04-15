/**
 * @file
 * @brief
 */

#ifndef __MAINDEV_HPP__
#define __MAINDEV_HPP__

#include "mbed.h"
#include "common/common.hpp"
#include "drivers/temperature/temperature.hpp"
#include "drivers/bmx055/bmx055.hpp"
#include "drivers/ina219/ina219.hpp"
#include "drivers/arm/arm.hpp"
#include "drivers/led/led.hpp"
#include "drivers/rpi/rpi.hpp"
#include "drivers/commdev/commdev.hpp"
#include "drivers/pinctrl/pinctrl.hpp"

class MainDevice {
public:
    MainDevice();
    ~MainDevice();

    I2C *      i2c;
    RawSerial *serial[2];

    Commdev *comm;
    Arm *    arm;
    DRV8830 *rw;
    RPi *    rpi;
    Tsens *  tsens;
    BMX055 * attsens;
    INA219 * powsens;

    LED *led;

    PinCtrl *power_rpi;
};

#endif // __MAINDEV_HPP__
