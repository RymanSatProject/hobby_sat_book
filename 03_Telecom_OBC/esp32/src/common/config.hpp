#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include <Arduino.h>
#include "../lib/lock/lock.hpp"

#define BAUD_LOG  115200
#define BAUD_HOST 115200
#define BAUD_RF   115200

#define PIN_BUZZER   19
#define PIN_HOST_IRQ 2

#define VERBOSITY VERB_INFO

#define RF_NAME "01-lite"

#define CW_VOLUME_PERCENT 3

#endif // __CONFIG_HPP__
