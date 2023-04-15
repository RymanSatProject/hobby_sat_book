#include "cw.hpp"

#include <string.h>

CW::CW() {
}

CW::~CW() {
}

void CW::construct(Buzzer *buzzer) {
    this->buzzer = buzzer;
}

static char int_to_ascii(uint8_t num) {
    if (num <= 9) {
        return '0' + num;
    } else if (num <= 0xF) {
        return 'A' + num - 0xA;
    } else {
        // Invalid input
        return 'Z';
    }
}

void CW::start(uint8_t *buf, uint16_t size) {
#ifdef DISABLE_BEACON
    // Do nothing
#else
    if (size > input_maxlen) {
        logger_error("CW: input too long\n");
        return;
    }

    const char header[] = "DE RSP01L ";
    const char footer[] = " \n";
    memset(message.buf, 0, sizeof(message.buf));
    memcpy(message.buf, header, strlen(header));
    message.len = strlen(header) + size * 2 + strlen(footer);
    message.pos = 0;

    uint16_t pos_ = strlen(header);

    for (int s = 0; s < size; s++) {
        message.buf[pos_++] = int_to_ascii(buf[s] >> 4);
        message.buf[pos_++] = int_to_ascii(buf[s] & 0xF);
    }

    memcpy(message.buf + pos_, footer, strlen(footer));

    logger_debug("CW start[%u] %s\n", message.len, message.buf);

    active = true;

#endif // DISABLE_BEACON
}

void CW::reset() {
    active = false;
}

void CW::tick() {
#ifdef DISABLE_BEACON
    // Do nothing
#else
    if (!active) {
        return;
    }

    morce(message.buf[message.pos++]);

    if (message.pos >= message.len) {
        reset();
        logger_debug("CW: done\n");
    }
#endif // DISABLE_BEACON
}

void CW::morce(char c) {
    if (c == ' ') {
        delay(interval_word);
    } else {
        delay(interval_letter);
        morce_char(c);
    }
}

struct char_morce {
    char        c;
    const char *morce;
};

const struct char_morce table[] = {
    {'A', ".-"},     {'B', "-..."},  {'C', "-.-."},  {'D', "-.."},
    {'E', "."},      {'F', "..-."},  {'G', "--."},   {'H', "...."},
    {'I', ".."},     {'J', ".---"},  {'K', "-.-"},   {'L', ".-.."},
    {'M', "--"},     {'N', "-."},    {'O', "---"},   {'P', ".--."},
    {'Q', "--.-"},   {'R', ".-."},   {'S', "..."},   {'T', "-"},
    {'U', "..-"},    {'V', "...-"},  {'W', ".--"},   {'X', "-..-"},
    {'Y', "-.--"},   {'Z', "--.."},  {'1', ".----"}, {'2', "..---"},
    {'3', "...--"},  {'4', "....-"}, {'5', "....."}, {'6', "-...."},
    {'7', "--..."},  {'8', "---.."}, {'9', "----."}, {'0', "-----"},
    {'\n', ".-.-."},
};

void CW::morce_char(char c) {
    for (size_t i = 0; i < ARRAY_SIZE(table); i++) {
        if (table[i].c == c) {
            morce_raw(table[i].morce);
        }
    }
}

void CW::morce_raw(const char *code) {
    uint8_t maxlen = strnlen(code, 5);

    for (uint8_t i = 0; i < maxlen; i++) {
        switch (code[i]) {
            case '.':
                buzzer->ring(short_ms);
                break;

            case '-':
                buzzer->ring(long_ms);
                break;
        }

        if (i < maxlen - 1) {
            delay(interval_sound);
        }
    }
}
