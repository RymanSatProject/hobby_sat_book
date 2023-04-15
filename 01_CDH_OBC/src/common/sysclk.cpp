/**
 * @file
 * @brief
 */

#include "sysclk.hpp"

Timer *SystemClock::timer;

SystemClock::SystemClock() {
    timer = new Timer;
    timer->start();
}

SystemClock::SystemClock(const SystemClock &obj) {
}

int SystemClock::msec(void) {
    static SystemClock instance;
    return instance.timer->read_ms();
}
