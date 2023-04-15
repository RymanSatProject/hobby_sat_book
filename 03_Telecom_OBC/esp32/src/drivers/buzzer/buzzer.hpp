#ifndef _BUZZER_HPP_
#define _BUZZER_HPP_

#include <Arduino.h>
#include <stdint.h>
#include "../../common/common.hpp"

class Buzzer {
public:
    Buzzer();
    ~Buzzer();

    void construct(uint8_t pin);

    void ring(uint32_t msec);
    void init(void);

private:
    uint8_t       pin;
    const int     freq = 5000;
    const uint8_t chan = 0;
    const int     resolution = 8;
    uint8_t       level;
};

#endif // _BUZZER_HPP_
