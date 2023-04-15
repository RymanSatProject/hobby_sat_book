/**
 * @file
 * @brief
 */

#ifndef __LED_HPP__
#define __LED_HPP__

#include "device/device.hpp"

class LED : public Device {
public:
    LED(PinName pin);
    ~LED();
    void blink(int on_ms, int off_ms);
    void flip(void);

private:
    DigitalOut *led;
};

#endif // __LED_HPP__
