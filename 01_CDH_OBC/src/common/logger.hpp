/**
 * @file
 * @brief
 */

#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include "mbed.h"
#include "config.hpp"

constexpr uint8_t VERB_FATAL = 1;
constexpr uint8_t VERB_ERROR = 2;
constexpr uint8_t VERB_WARNING = 3;
constexpr uint8_t VERB_INFO = 4;
constexpr uint8_t VERB_DEBUG = 5;
constexpr uint8_t VERB_TRACE = 6;
constexpr uint8_t VERB_HARDEST = VERB_FATAL;

#if VERBOSITY >= VERB_HARDEST

#define logger_base(...) printf(__VA_ARGS__)

int logger_config(uint8_t level);

uint8_t logger_verbosity(void);

template <typename... args_t>
void logger(uint8_t verb, const char *format, args_t const &...args) {
    if (verb <= logger_verbosity()) {
        logger_base(format, args...);
    }
}

#else

#define logger_config(...)
#define logger_base(...)

#endif // if VERBOSITY >= VERB_HARDEST

#if VERBOSITY >= VERB_TRACE
#define logger_trace(...) \
    (logger(VERB_TRACE, "[TRC] "), logger(VERB_TRACE, __VA_ARGS__))
#else
#define logger_trace(...)
#endif

#if VERBOSITY >= VERB_DEBUG
#define logger_debug(...) \
    (logger(VERB_DEBUG, "[DBG] "), logger(VERB_DEBUG, __VA_ARGS__))
#else
#define logger_debug(...)
#endif

#if VERBOSITY >= VERB_INFO
#define logger_info(...) \
    (logger(VERB_INFO, "[INF] "), logger(VERB_INFO, __VA_ARGS__))
#else
#define logger_info(...)
#endif

#if VERBOSITY >= VERB_WARNING
#define logger_warning(...) \
    (logger(VERB_WARNING, "[WRN] "), logger(VERB_WARNING, __VA_ARGS__))
#else
#define logger_warning(...)
#endif

#if VERBOSITY >= VERB_ERROR
#define logger_error(...) \
    (logger(VERB_ERROR, "[ERR] "), logger(VERB_ERROR, __VA_ARGS__))
#else
#define logger_error(...)
#endif

#if VERBOSITY >= VERB_FATAL
#define logger_fatal(...) \
    (logger(VERB_FATAL, "[FTL] "), logger(VERB_FATAL, __VA_ARGS__))
#else
#define logger_fatal(...)
#endif

void logger_hexdump(uint8_t verb, uint8_t buf[], uint32_t size);

#endif // __LOGGER_HPP__
