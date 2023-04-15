/**
 * @file
 * @brief
 */

#ifndef __LOCK_HPP__
#define __LOCK_HPP__

#include <Arduino.h>

class Lock {
public:
    Lock();
    ~Lock();
    int  lock(void);
    void unlock(void);

private:
    SemaphoreHandle_t semaphore;
};

#endif // __LOCK_HPP__
