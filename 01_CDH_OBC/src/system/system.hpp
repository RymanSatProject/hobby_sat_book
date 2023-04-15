/**
 * @file
 * @brief
 */

#ifndef __SYSTEM_HPP__
#define __SYSTEM_HPP__

#include "mbed.h"
#include "common/common.hpp"
#include "maindev/maindev.hpp"
#include "system/hkdata.hpp"
#include "command/command.hpp"
#include "power/power.hpp"

typedef struct _MainSystem {
    MainDevice * dev;
    Power *      power;
    HKdata *     hk;
    Command *    cmd;
    SpaceSocket *sock;
} MainSystem;

class SystemTask {
public:
    SystemTask(MainSystem *_sys, int _interval_ms);
    ~SystemTask();

    virtual void process(void) = 0;
    void         run(void);
    int          interval_ms;
    int          prev_ms;
    MainSystem * sys;
};

class HouseKeepingTask : public SystemTask {
public:
    HouseKeepingTask(MainSystem *_sys, int _interval_ms);
    void process(void);
};

class BeaconTask : public SystemTask {
public:
    BeaconTask(MainSystem *_sys, int _interval_ms);
    void process(void);
};

class LEDTask : public SystemTask {
public:
    LEDTask(MainSystem *_sys, int _interval_ms);
    void process(void);
};

class MainTask : public SystemTask {
public:
    MainTask(MainSystem *_sys, int _interval_ms);
    void process(void);
};

#endif // __SYSTEM_HPP__
