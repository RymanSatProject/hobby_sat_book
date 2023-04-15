#include "logger.hpp"
#include "../lib/lock/lock.hpp"
#include "config.hpp"

Lock           *lock;
HardwareSerial *serial;

void logger_init(uint32_t baud) {
    lock = new Lock;

#ifdef RF_UART
    serial = new HardwareSerial(1);
    const uint8_t rx = 26;
    const uint8_t tx = 27;
    serial->begin(baud, SERIAL_8N1, rx, tx);
#else
    serial = new HardwareSerial(0);
    serial->begin(baud);
#endif
}

void logger_base(const char *format, ...) {
    lock->lock();

    va_list args;
    va_start(args, format);

    for (int16_t i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%') {
            i += 1;
            switch (format[i]) {
                case 'b':
                    serial->print(va_arg(args, int), BIN);
                    break;

                case 'c':
                    serial->print((char)va_arg(args, int));
                    break;

                case 'd':
                case 'u': // FALLTHROUGH
                    serial->print(va_arg(args, int), DEC);
                    break;

                case 's':
                    serial->print(va_arg(args, char *));
                    break;

                case 'x': // FALLTHROUGH
                case 'X':
                    serial->print(va_arg(args, unsigned int), HEX);
                    break;

                case '%':
                    serial->print((char)format[i]);
                    break;

                default:
                    serial->print('?');
                    break;
            }

        } else {
            serial->print((char)format[i]);
        }
    }

    va_end(args);

    lock->unlock();
}

uint8_t logger_verbosity(void) {
    return VERBOSITY;
}

void logger_hexdump(uint8_t verb, uint8_t buf[], uint32_t size) {
    if (verb > logger_verbosity()) {
        return;
    }

    if (size == 0) {
        logger_base("(empty)\n");
        return;
    }

    int32_t       size_remaining = size;
    const uint8_t section = 16;
    for (uint16_t i = 0; size_remaining > 0; i++) {
        uint8_t nbytes = size_remaining < section ? size_remaining : section;
        for (uint16_t j = 0; j < nbytes; j++) {
            logger_base("%X ", buf[j + i * section]);
        }
        logger_base("\n");
        size_remaining -= section;
    }
}
