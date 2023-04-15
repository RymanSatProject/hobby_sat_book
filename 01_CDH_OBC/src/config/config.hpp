/**
 * @file
 * @brief
 */

#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#define VERBOSITY VERB_DEBUG

// DRIVE_*_LIMIT: 0 - 65535
#define DRIVE_FORWARD_LIMIT 100
#define DRIVE_REVERSE_LIMIT 65435
#define DRIVE_TIME_MS       30000

#include "stm32f401re.hpp"

#endif // __CONFIG_HPP__
