/**
 * @file
 * @brief
 */

#ifndef __DEVICE_HPP__
#define __DEVICE_HPP__

#include "mbed.h"
#include "common/common.hpp"

typedef enum _powstat : uint8_t {
    POWSTAT_OFF,
    POWSTAT_ON,
    POWSTAT_SHUTTING_DOWN,
    POWSTAT_WAKING_UP,
    POWSTAT_NONE = 0xFF,
} powstat_t;

/**
 * @brief Device model
 * @details This class should be the base class of all device drivers made upon
 * mbed platform drivers
 */
class Device {
public:
    Device();
    virtual ~Device();
    /**
     * @brief Initialize device
     */
    virtual int init(void);

    // OPTIONAL: power operations
    // Implement what has to be done to change power state
    // Return 0 on success, non-zero on failure
    virtual int wakeup(void);
    virtual int shutdown(void);

    /**
     * @brief monitor power status and transition if necessary
     */
    virtual void monitor(void);

    /**
     * @brief Register/unregister power management devices
     */
    void register_pm(Device *powdev);

    // optional: turn on or off the destination
    // Only used by power devices
    virtual int supply_on(int v = 0);
    virtual int supply_off(int v = 0);

    powstat_t powstat;

    const char *name;

protected:
    Device *powdev;
    Timer * powtimer;
};

#endif // __DEVICE_HPP__
