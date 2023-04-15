/**
 * @file
 * @brief
 */

#ifndef __TEMPERATURE_HPP__
#define __TEMPERATURE_HPP__

#include "device/device.hpp"

class Tsens : public Device {
public:
    Tsens(PinName pin, float v0);
    ~Tsens();

    /**
     * @brief return current temperature [degC]
     * @return float
     */
    float read(void);

    /**
     * @brief return raw voltage value
     */
    float raw_mv(void);

private:
    AnalogIn *adc;
    float     ref_mv;
};

#endif // __TEMPERATURE_HPP__
