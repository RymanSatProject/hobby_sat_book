#ifndef __SYSCLK_HPP__
#define __SYSCLK_HPP__

#include "mbed.h"
#include "common/common.hpp"

/**
 * @brief Global clock
 */
class SystemClock {
public:
    static int msec(void);

    static SystemClock* get_instance(void);

private:
    SystemClock();
    SystemClock(const SystemClock& obj);
    static Timer* timer;
};

#endif // __SYSCLK_HPP__
