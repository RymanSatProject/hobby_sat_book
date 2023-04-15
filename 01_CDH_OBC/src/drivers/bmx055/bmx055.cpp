#include "bmx055.hpp"

constexpr uint8_t BMX055_ACC_ADDR_ACCD_X_LSB = 0x02;
constexpr uint8_t BMX055_ACC_ADDR_PMU_RANGE = 0x0F;
constexpr uint8_t BMX055_ACC_ADDR_PMU_BW = 0x10;
constexpr uint8_t BMX055_ACC_ADDR_PMU_LPW = 0x11;

constexpr uint8_t BMX055_GYR_ADDR_RATE_X_LSB = 0x02;
constexpr uint8_t BMX055_GYR_ADDR_PMU_RANGE = 0x0F;
constexpr uint8_t BMX055_GYR_ADDR_PMU_BW = 0x10;
constexpr uint8_t BMX055_GYR_ADDR_PMU_LPW = 0x11;

constexpr uint8_t BMX055_MAG_ADDR_DATAX_LSB = 0x42;
constexpr uint8_t BMX055_MAG_ADDR_CTRL1 = 0x4B;
constexpr uint8_t BMX055_MAG_ADDR_CTRL2 = 0x4C;
constexpr uint8_t BMX055_MAG_ADDR_IRQ = 0x4D;
constexpr uint8_t BMX055_MAG_ADDR_CTRL3 = 0x4E;
constexpr uint8_t BMX055_MAG_ADDR_REPXY = 0x51;
constexpr uint8_t BMX055_MAG_ADDR_REPZ = 0x52;

constexpr uint8_t BMX055_ACC_RANGE_2G = 0x3;
constexpr uint8_t BMX055_ACC_RANGE_4G = 0x5;
constexpr uint8_t BMX055_ACC_RANGE_8G = 0x8;
constexpr uint8_t BMX055_ACC_RANGE_16G = 0xC;

constexpr uint8_t BMX055_ACC_PWR_MODE_NORMAL = 0x0 << 5;
constexpr uint8_t BMX055_ACC_PWR_MODE_DEEP_SUSPEND = 0x1 << 5;
constexpr uint8_t BMX055_ACC_PWR_MODE_LOW_POWER = 0x2 << 5;
constexpr uint8_t BMX055_ACC_PWR_MODE_SUSPEND = 0x4 << 5;
constexpr uint8_t BMX055_ACC_SLP_DUR_0P5MS = 0x5 << 1;
constexpr uint8_t BMX055_ACC_SLP_DUR_1MS = 0x6 << 1;
constexpr uint8_t BMX055_ACC_SLP_DUR_2MS = 0x7 << 1;
constexpr uint8_t BMX055_ACC_SLP_DUR_4MS = 0x8 << 1;
constexpr uint8_t BMX055_ACC_SLP_DUR_6MS = 0x9 << 1;
constexpr uint8_t BMX055_ACC_SLP_DUR_10MS = 0xA << 1;
constexpr uint8_t BMX055_ACC_SLP_DUR_25MS = 0xB << 1;
constexpr uint8_t BMX055_ACC_SLP_DUR_50MS = 0xC << 1;
constexpr uint8_t BMX055_ACC_SLP_DUR_100MS = 0xD << 1;
constexpr uint8_t BMX055_ACC_SLP_DUR_500MS = 0xE << 1;
constexpr uint8_t BMX055_ACC_SLP_DUR_1S = 0xF << 1;

constexpr uint8_t BMX055_GYR_RANGE_2000DEGS = 0x0;
constexpr uint8_t BMX055_GYR_RANGE_1000DEGS = 0x1;
constexpr uint8_t BMX055_GYR_RANGE_500DEGS = 0x2;
constexpr uint8_t BMX055_GYR_RANGE_250DEGS = 0x3;
constexpr uint8_t BMX055_GYR_RANGE_125DEGS = 0x4;

constexpr uint8_t BMX055_GYR_PWR_MODE_NORMAL = 0x0 << 5;
constexpr uint8_t BMX055_GYR_PWR_MODE_DEEP_SUSPEND = 0x1 << 5;
constexpr uint8_t BMX055_GYR_PWR_MODE_SUSPEND = 0x4 << 5;
constexpr uint8_t BMX055_GYR_SLP_DUR_2MS = 0x0 << 1;
constexpr uint8_t BMX055_GYR_SLP_DUR_4MS = 0x1 << 1;
constexpr uint8_t BMX055_GYR_SLP_DUR_5MS = 0x2 << 1;
constexpr uint8_t BMX055_GYR_SLP_DUR_8MS = 0x3 << 1;
constexpr uint8_t BMX055_GYR_SLP_DUR_10MS = 0x4 << 1;
constexpr uint8_t BMX055_GYR_SLP_DUR_15MS = 0x5 << 1;
constexpr uint8_t BMX055_GYR_SLP_DUR_18MS = 0x6 << 1;
constexpr uint8_t BMX055_GYR_SLP_DUR_20MS = 0x7 << 1;

constexpr uint8_t BMX055_MAG_CTRL1_SWRESET = 0x82;
constexpr uint8_t BMX055_MAG_CTRL1_SPI3EN = 0x4;
constexpr uint8_t BMX055_MAG_CTRL1_POW_ON = 0x1;
constexpr uint8_t BMX055_MAG_CTRL1_POW_OFF = 0x0;

constexpr uint8_t BMX055_MAG_CTRL2_OPMODE_NORMAL = 0x0 << 1;
constexpr uint8_t BMX055_MAG_CTRL2_OPMODE_FORCED = 0x1 << 1;
constexpr uint8_t BMX055_MAG_CTRL2_OPMODE_SLEEP = 0x3 << 1;

constexpr uint8_t BMX055_MAG_CTRL3_DATA_READY_PIN_EN = 1 << 7;
constexpr uint8_t BMX055_MAG_CTRL3_IRQ_PIN_ENABLE = 1 << 6;
constexpr uint8_t BMX055_MAG_CTRL3_CH_Z_ENABLE = 0 << 5;
constexpr uint8_t BMX055_MAG_CTRL3_CH_Z_DISABLE = 1 << 5;
constexpr uint8_t BMX055_MAG_CTRL3_CH_Y_ENABLE = 0 << 4;
constexpr uint8_t BMX055_MAG_CTRL3_CH_Y_DISABLE = 1 << 4;
constexpr uint8_t BMX055_MAG_CTRL3_CH_X_ENABLE = 0 << 3;
constexpr uint8_t BMX055_MAG_CTRL3_CH_X_DISABLE = 1 << 3;
constexpr uint8_t BMX055_MAG_CTRL3_DR_POL_HIGH = 1 << 2;
constexpr uint8_t BMX055_MAG_CTRL3_DR_POL_LOW = 0 << 2;
constexpr uint8_t BMX055_MAG_CTRL3_IRQ_LATCH_ENABLE = 1 << 1;
constexpr uint8_t BMX055_MAG_CTRL3_IRQ_POL_HIGH = 1 << 0;
constexpr uint8_t BMX055_MAG_CTRL3_IRQ_POL_LOW = 0 << 0;

// Address when JMP1-3 are open
constexpr uint8_t I2C_ADDR_BMX055_GYR = 0x69 << 1;
constexpr uint8_t I2C_ADDR_BMX055_ACC = 0x19 << 1;
constexpr uint8_t I2C_ADDR_BMX055_MAG = 0x13 << 1;

constexpr int BMX055_REG_WRITE_WAIT_US = 100000;

BMX055::BMX055(I2C *i2c) {
    name = "BMX055";
    this->i2c = i2c;
    addr_acc = I2C_ADDR_BMX055_ACC;
    addr_gyr = I2C_ADDR_BMX055_GYR;
    addr_mag = I2C_ADDR_BMX055_MAG;

    init();
}

BMX055::~BMX055() {
}

int BMX055::init(void) {
    int err = 0;

    err = init_acc(BMX055_ACC_RANGE_2G, BMX055_ACC_BW_125HZ);
    if (err) {
        goto exit;
    }
    err = init_gyr(BMX055_GYR_RANGE_125DEGS, BMX055_GYR_BW_ODR100_FIL32);
    if (err) {
        goto exit;
    }

    err = init_mag(BMX055_MAG_CTRL2_ODR_20HZ);
    if (err) {
        goto exit;
    }

    online = true;

exit:
    return 0;
}

int BMX055::init_acc(uint8_t range, bmx055_acc_bw_t bandwidth) {
    if (write_config(I2C_ADDR_BMX055_ACC, BMX055_ACC_ADDR_PMU_RANGE, range)) {
        return -EIO;
    }

    if (write_config(I2C_ADDR_BMX055_ACC, BMX055_ACC_ADDR_PMU_BW, bandwidth)) {
        return -EIO;
    }

    if (write_config(I2C_ADDR_BMX055_ACC, BMX055_ACC_ADDR_PMU_LPW,
                     BMX055_ACC_PWR_MODE_NORMAL | BMX055_ACC_SLP_DUR_0P5MS)) {
        return -EIO;
    }

    const float bitwidth = 2048.0;

    switch (range) {
        case BMX055_ACC_RANGE_2G:
            coef_acc = 2.0 / bitwidth;
            break;

        case BMX055_ACC_RANGE_4G:
            coef_acc = 4.0 / bitwidth;
            break;

        case BMX055_ACC_RANGE_8G:
            coef_acc = 8.0 / bitwidth;
            break;

        case BMX055_ACC_RANGE_16G:
            coef_acc = 16.0 / bitwidth;
            break;

        default:
            logger_error("[%s] Invalid range: 0x%X\n", __func__, range);
            coef_acc = 0.0;
            break;
    }

    return 0;
}

int BMX055::init_gyr(uint8_t range, bmx055_gyr_bw_t bandwidth) {
    if (write_config(I2C_ADDR_BMX055_GYR, BMX055_GYR_ADDR_PMU_RANGE, range)) {
        return -EIO;
    }

    if (write_config(I2C_ADDR_BMX055_GYR, BMX055_GYR_ADDR_PMU_BW, bandwidth)) {
        return -EIO;
    }

    if (write_config(I2C_ADDR_BMX055_GYR, BMX055_GYR_ADDR_PMU_LPW,
                     BMX055_GYR_PWR_MODE_NORMAL | BMX055_GYR_SLP_DUR_2MS)) {
        return -EIO;
    }

    const float bitwidth = 32768.0;

    switch (range) {
        case BMX055_GYR_RANGE_2000DEGS:
            coef_gyr = 2000.0 / bitwidth;
            break;

        case BMX055_GYR_RANGE_1000DEGS:
            coef_gyr = 1000.0 / bitwidth;
            break;

        case BMX055_GYR_RANGE_500DEGS:
            coef_gyr = 500.0 / bitwidth;
            break;

        case BMX055_GYR_RANGE_250DEGS:
            coef_gyr = 250.0 / bitwidth;
            break;

        case BMX055_GYR_RANGE_125DEGS:
            coef_gyr = 125.0 / bitwidth;
            break;

        default:
            logger_error("[%s] Invalid range: %u\n", __func__, range);
            coef_gyr = 0.0;
            break;
    }

    return 0;
}

int BMX055::init_mag(bmx055_mag_bw_t bandwidth) {
    write_config(I2C_ADDR_BMX055_MAG, BMX055_MAG_ADDR_CTRL1,
                 BMX055_MAG_CTRL1_SWRESET | BMX055_MAG_CTRL1_POW_ON);
    wait_us(BMX055_REG_WRITE_WAIT_US);

    write_config(I2C_ADDR_BMX055_MAG, BMX055_MAG_ADDR_CTRL1,
                 BMX055_MAG_CTRL1_POW_ON);
    wait_us(BMX055_REG_WRITE_WAIT_US);

    write_config(I2C_ADDR_BMX055_MAG, BMX055_MAG_ADDR_CTRL2,
                 bandwidth | BMX055_MAG_CTRL2_OPMODE_NORMAL);

    write_config(
        I2C_ADDR_BMX055_MAG, BMX055_MAG_ADDR_CTRL3,
        BMX055_MAG_CTRL3_DATA_READY_PIN_EN | BMX055_MAG_CTRL3_CH_Z_ENABLE |
            BMX055_MAG_CTRL3_CH_Y_ENABLE | BMX055_MAG_CTRL3_CH_X_ENABLE |
            BMX055_MAG_CTRL3_DR_POL_HIGH);

    write_config(I2C_ADDR_BMX055_MAG, BMX055_MAG_ADDR_REPXY, 0x1);
    write_config(I2C_ADDR_BMX055_MAG, BMX055_MAG_ADDR_REPZ, 0x2);

    return 0;
}

int BMX055::write_config(uint8_t addr_chip, uint8_t addr_reg, uint8_t data) {
    uint8_t buf[2] = {addr_reg, data};

    return i2c->write(addr_chip, (char *)buf, ARRAY_SIZE(buf));
}

int BMX055::read(uint8_t addr_chip, uint8_t addr_reg, uint8_t data[6]) {
    uint8_t buf_write[1] = {addr_reg};

    if (i2c->write(addr_chip, (char *)buf_write, ARRAY_SIZE(buf_write))) {
        return -EIO;
    }

    if (i2c->read(addr_chip, (char *)data, 6)) {
        return -EIO;
    }

    return 0;
}

int BMX055::get_acc(bmx055_sensor_data_t &acc) {
    uint8_t data[6];
    int16_t acc_raw[3];

    int err = read(I2C_ADDR_BMX055_ACC, BMX055_ACC_ADDR_ACCD_X_LSB, data);
    if (err) {
        return err;
    }

    for (uint8_t i = 0; i < 3; i++) {
        acc_raw[i] = (int16_t)((data[2 * i + 1] << 8) | (data[2 * i] & 0xF0));
    }

    acc.x = coef_acc * (acc_raw[0] >> 4);
    acc.y = coef_acc * (acc_raw[1] >> 4);
    acc.z = coef_acc * (acc_raw[2] >> 4);

    return 0;
}

int BMX055::get_gyr(bmx055_sensor_data_t &gyr) {
    uint8_t data[6];
    int16_t gyr_raw[3];

    int err = read(I2C_ADDR_BMX055_GYR, BMX055_GYR_ADDR_RATE_X_LSB, data);
    if (err) {
        return err;
    }

    for (uint8_t i = 0; i < 3; i++) {
        gyr_raw[i] = (int16_t)((data[2 * i + 1] << 8) | data[2 * i]);
    }

    gyr.x = coef_gyr * gyr_raw[0];
    gyr.y = coef_gyr * gyr_raw[1];
    gyr.z = coef_gyr * gyr_raw[2];

    return 0;
}

int BMX055::get_mag(bmx055_sensor_data_t &mag) {
    uint8_t data[6];
    int16_t mag_raw[3];

    int err = read(I2C_ADDR_BMX055_MAG, BMX055_MAG_ADDR_DATAX_LSB, data);
    if (err) {
        return err;
    }

    mag_raw[0] = (int16_t)((data[1] << 8) | (data[0] & 0xF8));
    mag_raw[1] = (int16_t)((data[3] << 8) | (data[2] & 0xF8));
    mag_raw[2] = (int16_t)((data[5] << 8) | (data[4] & 0xFE));

    mag.x = mag_raw[0] / 16.0;
    mag.y = mag_raw[1] / 16.0;
    mag.z = mag_raw[2] / 2.0;

    return 0;
}
