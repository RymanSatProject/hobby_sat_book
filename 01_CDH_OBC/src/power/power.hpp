/**
 * @file
 * @brief
 */

#ifndef __POWER_HPP__
#define __POWER_HPP__

#include "common/common.hpp"
#include "maindev/maindev.hpp"

typedef enum _devid : uint8_t { DEVID_RPI, DEVID_NUM } devid_t;

/**
 * @brief Manages system power status
 */
class Power {
public:
    Power(MainDevice* dev);
    ~Power();

    void run(void);

private:
    MainDevice* dev;
};

#endif // __POWER_HPP__
