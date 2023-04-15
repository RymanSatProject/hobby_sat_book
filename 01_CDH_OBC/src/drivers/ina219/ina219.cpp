#include "ina219.hpp"
#include "common/logger.hpp"

// A0 = A1 = GND
constexpr uint8_t I2C_ADDR_INA219 = 0x40 << 1;

constexpr uint8_t INA219_ADDR_CONFIG = 0x0;
constexpr uint8_t INA219_ADDR_SHUNT_VOL = 0x1;
constexpr uint8_t INA219_ADDR_BUS_VOL = 0x2;
constexpr uint8_t INA219_ADDR_POW = 0x3;
constexpr uint8_t INA219_ADDR_CUR = 0x4;
constexpr uint8_t INA219_ADDR_CAL = 0x5;

constexpr uint8_t INA219_ADC_9BIT = 0x0 << 3;
constexpr uint8_t INA219_ADC_10BIT = 0x1 << 3;

constexpr uint16_t INA219_CONFIG_RESET = 1u << 15;
constexpr uint16_t INA219_CONFIG_BUS_RANGE_32V = 1u << 13;
constexpr uint16_t INA219_CONFIG_BUS_RANGE_16V = 0u << 13;
constexpr uint16_t INA219_CONFIG_GAIN_1_40MV = 0u << 11;
constexpr uint16_t INA219_CONFIG_GAIN_2_80MV = 1u << 11;
constexpr uint16_t INA219_CONFIG_GAIN_4_160MV = 2u << 11;
constexpr uint16_t INA219_CONFIG_GAIN_8_320MV = 3u << 11;
constexpr uint16_t INA219_CONFIG_BADC_RES_9BIT = 0u << 7;
constexpr uint16_t INA219_CONFIG_BADC_RES_10BIT = 1u << 7;
constexpr uint16_t INA219_CONFIG_BADC_RES_11BIT = 2u << 7;
constexpr uint16_t INA219_CONFIG_BADC_RES_12BIT = 3u << 7;
constexpr uint16_t INA219_CONFIG_BADC_RES_2_1P06MS = 9u << 7;
constexpr uint16_t INA219_CONFIG_BADC_RES_4_2P13MS = 10u << 7;
constexpr uint16_t INA219_CONFIG_BADC_RES_8_4P26MS = 11u << 7;
constexpr uint16_t INA219_CONFIG_BADC_RES_16_8P51MS = 12u << 7;
constexpr uint16_t INA219_CONFIG_BADC_RES_32_17P02MS = 13u << 7;
constexpr uint16_t INA219_CONFIG_BADC_RES_64_34P05MS = 14u << 7;
constexpr uint16_t INA219_CONFIG_BADC_RES_128_68P10MS = 15u << 7;
constexpr uint16_t INA219_CONFIG_ADC_RES_9BIT = 0u << 3;
constexpr uint16_t INA219_CONFIG_SADC_RES_10BIT = 1u << 3;
constexpr uint16_t INA219_CONFIG_SADC_RES_11BIT = 2u << 3;
constexpr uint16_t INA219_CONFIG_SADC_RES_12BIT = 3u << 3;
constexpr uint16_t INA219_CONFIG_SADC_RES_2_1P06MS = 9u << 3;
constexpr uint16_t INA219_CONFIG_SADC_RES_4_2P13MS = 10u << 3;
constexpr uint16_t INA219_CONFIG_SADC_RES_8_4P26MS = 11u << 3;
constexpr uint16_t INA219_CONFIG_SADC_RES_16_8P51MS = 12u << 3;
constexpr uint16_t INA219_CONFIG_SADC_RES_32_17P02MS = 13u << 3;
constexpr uint16_t INA219_CONFIG_SADC_RES_64_34P05MS = 14u << 3;
constexpr uint16_t INA219_CONFIG_SADC_RES_128_68P10MS = 15u << 3;
constexpr uint16_t INA219_CONFIG_MODE_PD = 0u << 0;
constexpr uint16_t INA219_CONFIG_MODE_SHUNT_TRIG = 1u << 0;
constexpr uint16_t INA219_CONFIG_MODE_BUS_TRIG = 2u << 0;
constexpr uint16_t INA219_CONFIG_MODE_SHUNT_BUS_TRIG = 3u << 0;
constexpr uint16_t INA219_CONFIG_MODE_ADC_OFF = 4u << 0;
constexpr uint16_t INA219_CONFIG_MODE_SHUNT_CONT = 5u << 0;
constexpr uint16_t INA219_CONFIG_MODE_BUS_CONT = 6u << 0;
constexpr uint16_t INA219_CONFIG_MODE_SHUNT_BUS_CONT = 7u << 0;

INA219::INA219(I2C* i2c) {
    name = "INA219";
    this->i2c = i2c;
    init();
}

INA219::~INA219() {
    reset();
}

/**
 * @brief Initializes INA219
 */
int INA219::init(void) {
    i2caddr = I2C_ADDR_INA219;

    // Max range: 2000 mA, 32V
    // (shunt resistance = 0.1 Ohm, gain = x8, 320mV)
    calreg_val = 0x1000;
    coef_cur = 0.1;
    coef_pow = 2;
    if (write(INA219_ADDR_CAL, 0x1000)) {
        return -EIO;
    }

    uint16_t config = INA219_CONFIG_BUS_RANGE_32V | INA219_CONFIG_GAIN_8_320MV |
                      INA219_CONFIG_BADC_RES_12BIT |
                      INA219_CONFIG_SADC_RES_12BIT |
                      INA219_CONFIG_MODE_SHUNT_BUS_CONT;
    if (write(INA219_ADDR_CONFIG, config)) {
        return -EIO;
    }

    online = true;

    return 0;
}

void INA219::reset(void) {
    write(INA219_ADDR_CONFIG, INA219_CONFIG_RESET);
}

float INA219::shunt_vol_mv(void) {
    uint16_t raw;
    read(INA219_ADDR_SHUNT_VOL, raw);
    logger_trace("INA219.%s: raw = 0x%02X\n", __func__, raw);

    // 10 uV/bit
    return 0.01 * (int16_t)raw;
}

float INA219::bus_vol_mv(void) {
    uint16_t raw;
    read(INA219_ADDR_BUS_VOL, raw);

    logger_trace("INA219.%s: raw = 0x%02X\n", __func__, raw);

    // Valid data is bit[:3]; unit = 4 mV/bit
    return (int16_t)(raw >> 3) * 4;
}

float INA219::power_mw(void) {
    if (write(INA219_ADDR_CAL, calreg_val)) {
        return 0.0;
    }

    uint16_t raw;
    if (read(INA219_ADDR_POW, raw)) {
        return 0.0;
    }

    logger_trace("INA219.%s: raw = 0x%02X\n", __func__, raw);

    return raw * coef_pow;
}

float INA219::current_ma(void) {
    if (write(INA219_ADDR_CAL, calreg_val)) {
        return 0.0;
    }

    uint16_t raw;
    if (read(INA219_ADDR_CUR, raw)) {
        return 0.0;
    }

    return raw * coef_cur * static_cast<int16_t>(raw);
}

int INA219::write(uint8_t addr_reg, uint16_t val) {
    uint8_t buf[3];

    buf[0] = addr_reg;
    buf[1] = (val >> 8) & 0xFF;
    buf[2] = val & 0xFF;
    return i2c->write(i2caddr, (char*)buf, 3);
}

int INA219::read(uint8_t addr_reg, uint16_t& data) {
    i2c->write(i2caddr, (char*)&addr_reg, 1);

    char buf[2];

    if (i2c->read(i2caddr, buf, 2)) {
        data = 0;
        return -EIO;
    } else {
        data = (buf[0] << 8) | buf[1];
        return 0;
    }
}
