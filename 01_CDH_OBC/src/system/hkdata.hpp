/**
 * @file
 * @brief
 */

#ifndef __HKDATA_HPP__
#define __HKDATA_HPP__

#include "mbed.h"
#include "maindev/maindev.hpp"
#include "power/power.hpp"

typedef struct _hkunit {
    int32_t  current_ms;
    int32_t  temp;
    int32_t  acc[3];
    int32_t  gyr[3];
    int32_t  mag[3];
    uint16_t power_mw;
    uint16_t current_ma;
    uint16_t bus_vol_mv;
    uint8_t  powstat;
} hkunit_t;

class HKdata {
public:
    HKdata(MainDevice *dev, Power *power, int size);
    ~HKdata();

    hkunit_t *data;
    hkunit_t *latest;
    int       idx;

    void update(void);
    void serialize(uint8_t buf[], int idx);

    uint8_t       size;
    static const uint8_t unitsize = 53;

private:
    MainDevice *dev;
    Power *     power;
};

#endif // __HKDATA_HPP__
