/**
 * @file GPIO as a power toggle
 * @brief
 */

#ifndef __PINCTRL_HPP__
#define __PINCTRL_HPP__

#include "device/device.hpp"

class PinCtrl : public Device {
public:
    PinCtrl(PinName pin);
    ~PinCtrl();

    int supply_on(int v = 0);
    int supply_off(int v = 0);

private:
    DigitalOut *dout;
};

#endif // __PINCTRL_HPP__
