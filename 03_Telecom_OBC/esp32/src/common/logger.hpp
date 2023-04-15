/**
 * @file
 * @brief
 */

#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include <Arduino.h>
#include "config.hpp"

#define VERB_FATAL   1
#define VERB_ERROR   2
#define VERB_WARNING 3
#define VERB_INFO    4
#define VERB_DEBUG   5
#define VERB_TRACE   6
#define VERB_HARDEST VERB_FATAL

#if !defined(VERBOSITY)
#error "VERBOSITY undefined"
#endif

#if VERBOSITY >= VERB_HARDEST

void logger_base(const char *format, ...);
void logger_init(uint32_t baud);

uint8_t logger_verbosity(void);

#else

#define logger_base(...)
#define logger_init(...)

#endif // if VERBOSITY >= VERB_HARDEST

#if VERBOSITY >= VERB_TRACE
#define logger_trace(...) (logger_base("[TRC] "), logger_base(__VA_ARGS__))
#else
#define logger_trace(...)
#endif

#if VERBOSITY >= VERB_DEBUG
#define logger_debug(...) (logger_base("[DBG] "), logger_base(__VA_ARGS__))
#else
#define logger_debug(...)
#endif

#if VERBOSITY >= VERB_INFO
#define logger_info(...) (logger_base("[INF] "), logger_base(__VA_ARGS__))
#else
#define logger_info(...)
#endif

#if VERBOSITY >= VERB_WARNING
#define logger_warning(...) (logger_base("[WRN] "), logger_base(__VA_ARGS__))
#else
#define logger_warning(...)
#endif

#if VERBOSITY >= VERB_ERROR
#define logger_error(...) (logger_base("[ERR] "), logger_base(__VA_ARGS__))
#else
#define logger_error(...)
#endif

#if VERBOSITY >= VERB_FATAL
#define logger_fatal(...) (logger_base("[FTL] "), logger_base(__VA_ARGS__))
#else
#define logger_fatal(...)
#endif

void logger_hexdump(uint8_t verb, uint8_t buf[], uint32_t size);

#endif // __LOGGER_HPP__
