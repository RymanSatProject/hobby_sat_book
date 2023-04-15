/**
 * @file
 */

#include "pinctrl.hpp"

PinCtrl::PinCtrl(PinName pin) {
    this->dout = new DigitalOut(pin);
    supply_off();
}

PinCtrl::~PinCtrl() {
    delete dout;
}

int PinCtrl::supply_on(int v) {
    *dout = 1;
    return 0;
}

int PinCtrl::supply_off(int v) {
    *dout = 0;
    return 0;
}
