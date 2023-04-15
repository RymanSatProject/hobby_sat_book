#ifndef _CW_HPP_
#define _CW_HPP_

#include <Arduino.h>
#include <stdint.h>
#include "../common/common.hpp"
#include "../drivers/buzzer/buzzer.hpp"

class CW {
public:
    CW();
    ~CW();

    void construct(Buzzer *buzzer);
    void start(uint8_t *buf, uint16_t size);
    void tick();
    void reset();

private:
    Buzzer *buzzer;

    void morce(char c);
    void morce_char(char c);
    void morce_raw(const char *code);

    const uint16_t        unit_ms = 60;
    const uint16_t        long_ms = unit_ms * 3;
    const uint16_t        short_ms = unit_ms;
    const uint16_t        interval_sound = unit_ms;
    const uint16_t        interval_letter = unit_ms * 3;
    const uint16_t        interval_word = unit_ms * 7;
    static const uint16_t input_maxlen = 100;
    static const uint16_t cw_maxlen = input_maxlen * 2 + 15;

    bool active = false;

    struct {
        char     buf[cw_maxlen];
        uint16_t pos;
        uint16_t len;

    } message;
};

#endif // _CW_HPP_
