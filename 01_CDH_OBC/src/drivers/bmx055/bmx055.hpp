/**
 * @file
 * @brief
 */

#ifndef __BMX055_HPP__
#define __BMX055_HPP__

#include "mbed.h"
#include "drivers/i2c/i2c_device.hpp"

// Written to BMX055_ACC_ADDR_PMU_BW reg
typedef enum _mbx055_acc_bw : uint8_t {
    BMX055_ACC_BW_7P81HZ = 0x8,
    BMX055_ACC_BW_15P63HZ = 0x9,
    BMX055_ACC_BW_31P25HZ = 0xA,
    BMX055_ACC_BW_62P5HZ = 0xB,
    BMX055_ACC_BW_125HZ = 0xC,
    BMX055_ACC_BW_250HZ = 0xD,
    BMX055_ACC_BW_500HZ = 0xE,
    BMX055_ACC_BW_2000HZ = 0xF
} bmx055_acc_bw_t;

// Written to BMX055_GYR_ADDR_PMU_BW reg
// ODRx_FILy: output data rate = x Hz, filter bandwidth = y Hz
typedef enum bmx055_gyr_bw : uint8_t {
    BMX055_GYR_BW_ODR100_FIL32 = 0x7,
    BMX055_GYR_BW_ODR200_FIL64 = 0x6,
    BMX055_GYR_BW_ODR100_FIL12 = 0x5,
    BMX055_GYR_BW_ODR200_FIL23 = 0x4,
    BMX055_GYR_BW_ODR400_FIL47 = 0x3,
    BMX055_GYR_BW_ODR1000_FIL116 = 0x2,
    BMX055_GYR_BW_ODR2000_FIL230 = 0x1,
    BMX055_GYR_BW_ODR2000_FIL523 = 0x0
} bmx055_gyr_bw_t;

// Written to BMX055_MAG_ADDR_CTRL2 reg
typedef enum bmx055_mag_bw : uint8_t {
    BMX055_MAG_CTRL2_ODR_10HZ = (0x0 << 3),
    BMX055_MAG_CTRL2_ODR_2HZ = (0x1 << 3),
    BMX055_MAG_CTRL2_ODR_6HZ = (0x2 << 3),
    BMX055_MAG_CTRL2_ODR_8HZ = (0x3 << 3),
    BMX055_MAG_CTRL2_ODR_15HZ = (0x4 << 3),
    BMX055_MAG_CTRL2_ODR_20HZ = (0x5 << 3),
    BMX055_MAG_CTRL2_ODR_25HZ = (0x6 << 3),
    BMX055_MAG_CTRL2_ODR_30HZ = (0x7 << 3)
} bmx055_mag_bw_t;

typedef struct {
    float x;
    float y;
    float z;
} bmx055_sensor_data_t;

class BMX055 : public I2CDevice {
public:
    BMX055(I2C *_i2c);
    ~BMX055();

    int init(void);

    /**
     * @brief Gets acceleration
     * @param acc: obtained value
     * @details unit = g
     */
    int get_acc(bmx055_sensor_data_t &acc);

    /**
     * @brief Gets angular velocity
     * @param gyr: obtained value
     * @details unit = deg/sec
     */
    int get_gyr(bmx055_sensor_data_t &gyr);

    /**
     * @brief Gets magnetism
     * @param mag: obtained value
     * @details unit = uT
     */
    int get_mag(bmx055_sensor_data_t &mag);

private:
    uint8_t addr_acc, addr_gyr, addr_mag;
    float   coef_acc, coef_gyr, coef_mag;

    int init_acc(uint8_t range, bmx055_acc_bw_t bandwidth);
    int init_gyr(uint8_t range, bmx055_gyr_bw_t bandwidth);
    int init_mag(bmx055_mag_bw_t bandwidth);
    int write_config(uint8_t addr_chip, uint8_t addr_reg, uint8_t data);
    int read(uint8_t addr_chip, uint8_t addr_reg, uint8_t data[6]);
};

#endif // __BMX055_HPP__
