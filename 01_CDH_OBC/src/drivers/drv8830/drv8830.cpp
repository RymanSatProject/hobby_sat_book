/**
 * @file
 */

#include "drv8830.hpp"

DRV8830::DRV8830(I2C *i2c, pinstat_t a0, pinstat_t a1) {
    name = "DRV8830";
    this->i2c = i2c;

    uint8_t num_a0 = (a0 == PINSTAT_LOW) ? 0 : (a0 == PINSTAT_HIZ) ? 2 : 4;
    uint8_t num_a1 = (a1 == PINSTAT_LOW) ? 0 : (a1 == PINSTAT_HIZ) ? 6 : 12;

    i2caddr = 0xC0 + num_a0 + num_a1;

    stop();
}

DRV8830::~DRV8830() {
    stop();
}

int DRV8830::init(void) {
    if (!this->is_online()) {
        logger_error("%s.%s: device not found\n", name, __func__);
        return -EIO;
    }

    return 0;
}

void DRV8830::standby(void) {
    control(VOUT_MIN_MV, MODE_STANDBY);
    clear_fault();
}

void DRV8830::brake(void) {
    control(VOUT_MIN_MV, MODE_BRAKE);
}

void DRV8830::stop(void) {
    brake();
    standby();
}

int DRV8830::control(uint32_t mv, uint8_t mode) {
    if (mode > MODE_MAX) {
        logger_error("[%s] Mode out of range: %u\n", __func__, mode);
        return -1;
    }

    if (mv < VOUT_MIN_MV || VOUT_MAX_MV < mv) {
        logger_error("%s: Voltage out of range: %u\n", __func__, mv);
        return -1;
    }

    uint8_t v_reg = mv_to_reg(mv);
    uint8_t cmd = (v_reg << 2) | mode;

    logger_trace("%s: mv = %u (%02X)\n", __func__, mv, v_reg);

    char payload[2] = {(char)REG_CONTROL, (char)cmd};

    return i2c->write(i2caddr, payload, 2);
}

void DRV8830::clear_fault() {
    char payload[2] = {(char)REG_FAULT, (char)0x80};
    i2c->write(i2caddr, payload, 2);
}

uint8_t DRV8830::fault_stat() {
    char payload[1] = {(char)REG_FAULT};

    i2c->write(i2caddr, payload, 1);
    i2c->read(i2caddr, payload, 1);

    return payload[0];
}

uint8_t DRV8830::mv_to_reg(uint32_t mv) {
    return VOUT_MIN_REG + (VOUT_MAX_REG - VOUT_MIN_REG) * (mv - VOUT_MIN_MV) /
                              (VOUT_MAX_MV - VOUT_MIN_MV);
}

uint32_t DRV8830::reg_to_mv(uint8_t reg) {
    return VOUT_MIN_MV + (VOUT_MAX_MV - VOUT_MIN_MV) * (reg - VOUT_MIN_REG) /
                             (VOUT_MAX_REG - VOUT_MIN_REG);
}
