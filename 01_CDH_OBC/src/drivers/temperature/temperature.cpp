/**
 * @file
 */

#include "temperature.hpp"

Tsens::Tsens(PinName pin, float ref_mv) {
    this->adc = new AnalogIn(pin);
    this->ref_mv = ref_mv;
}

Tsens::~Tsens() {
    delete adc;
}

float Tsens::read(void) {
    // Following LM61 specification
    return (raw_mv() * 1000 - 600) / 10;
}

float Tsens::raw_mv(void) {
    return adc->read() * ref_mv;
}
