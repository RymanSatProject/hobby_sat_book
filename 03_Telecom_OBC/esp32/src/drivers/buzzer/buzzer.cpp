#include "buzzer.hpp"

Buzzer::Buzzer() {
}

Buzzer::~Buzzer() {
}

void Buzzer::construct(uint8_t pin) {
    this->pin = pin;
    init();
}

void Buzzer::init(void) {
    ledcSetup(chan, freq, resolution);
    ledcAttachPin(pin, chan);

    const uint8_t maxlevel = 255;
    float         rate = 0.01 * CW_VOLUME_PERCENT;
    level = int(maxlevel * rate);

    logger_info("Sound level: %u %\n", level);
}

void Buzzer::ring(uint32_t msec) {
    ledcWrite(0, level);
    delay(msec);
    ledcWrite(0, 0);
}
