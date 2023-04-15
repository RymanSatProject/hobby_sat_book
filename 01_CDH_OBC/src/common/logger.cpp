#include "logger.hpp"

#if VERBOSITY >= VERB_HARDEST

volatile static uint8_t _verbosity;

int logger_config(uint8_t level) {
    switch (level) {
        case VERB_FATAL:
            logger_base("Level = FATAL\n");
            break;

        case VERB_ERROR:
            logger_base("Level = ERROR\n");
            break;

        case VERB_WARNING:
            logger_base("Level = WARNING\n");
            break;

        case VERB_INFO:
            logger_base("Level = INFO\n");
            break;

        case VERB_DEBUG:
            logger_base("Level = DEBUG\n");
            break;

        case VERB_TRACE:
            logger_base("Level = TRACE\n");
            break;

        default:
            logger_base("Level invalid\n");
            return -1;
    }

    _verbosity = level;
    return 0;
}

uint8_t logger_verbosity(void) {
    return _verbosity;
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
            logger_base("%02X ", buf[j + i * section]);
        }
        logger_base("\n");
        size_remaining -= section;
    }
}

#endif // if VERBOSITY >= VERB_HARDEST
