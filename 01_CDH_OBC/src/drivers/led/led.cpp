/**
 * @file
 */

#include "led.hpp"

LED::LED(PinName pin) {
    led = new DigitalOut(pin);
}

LED::~LED() {
    delete led;
}

void LED::blink(int on_ms, int off_ms) {
    *led = 1;
    wait_us(1000 * on_ms);
    *led = 0;
    wait_us(1000 * off_ms);
}

void LED::flip(void) {
    *led = (led->read() == 0) ? 1 : 0;
}
