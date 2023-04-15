/**
 * @file
 * @brief カメラアームを扱うクラスを定義します。
 */

#ifndef __DRV8830_HPP__
#define __DRV8830_HPP__

#include "drivers/i2c/i2c_device.hpp"

class DRV8830 : public I2CDevice {
public:
    DRV8830(I2C *i2c, pinstat_t a0, pinstat_t a1);
    ~DRV8830();

    int init(void);

    void    standby(void);
    void    brake(void);
    void    stop(void);
    int     control(uint32_t mv, uint8_t mode);
    uint8_t fault_stat(void);
    void    clear_fault(void);

    /**
     * @brief Convert from mV to value written in the IC
     */
    uint8_t  mv_to_reg(uint32_t mv);
    uint32_t reg_to_mv(uint8_t reg);

    static const uint8_t REG_CONTROL = 0x00;
    static const uint8_t REG_FAULT = 0x01;
    static const uint8_t MODE_STANDBY = 0x00;
    static const uint8_t MODE_REVERSE = 0x01;
    static const uint8_t MODE_FORWARD = 0x02;
    static const uint8_t MODE_BRAKE = 0x03;
    static const uint8_t MODE_MAX = 0x03;

    static const uint8_t  VOUT_MAX_REG = 0x3F;
    static const uint8_t  VOUT_MIN_REG = 0x06;
    static const uint32_t VOUT_MAX_MV = 5060;
    static const uint32_t VOUT_MIN_MV = 480;
    static const uint32_t VOUT_STEP_MV = 80;

    static const uint8_t FAULT_I2C = BIT(7);
    static const uint8_t FAULT_OCCURRED = BIT(0);
};

#endif // __DRV8830_HPP__
