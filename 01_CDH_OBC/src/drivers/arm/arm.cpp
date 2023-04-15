/**
 * @file
 * @brief
 */

#include "arm.hpp"

constexpr uint8_t POTENTIOMETER_ERROR = 0x40;

constexpr uint32_t VOUT_LIMIT_MV = 3050;
constexpr uint16_t STEP_INTERVAL_TIME_US = 50000;

constexpr uint16_t DRIVE_FORWARD_ERROR_GUARD = 65535;
constexpr uint16_t DRIVE_FORWARD_BRAKE_POINT = DRIVE_FORWARD_LIMIT + 1000;
constexpr uint16_t DRIVE_REVERSE_BRAKE_POINT = DRIVE_REVERSE_LIMIT - 1000;
constexpr uint16_t DRIVE_REVERSE_ERROR_GUARD = 0;

constexpr uint8_t FAULT_MASK = 0x3C;

constexpr uint8_t RET_ERROR = BIT(0);
constexpr uint8_t RET_ERROR_I2C = BIT(7);

Arm::Arm(I2C *i2c, pinstat_t a0, pinstat_t a1, PinName pin_pos)
    : DRV8830(i2c, a0, a1) {
    name = "Camera arm";
    posmeter = new AnalogIn(pin_pos);
    timer = new Timer;
}

Arm::~Arm() {
    delete timer;
    delete posmeter;
}

uint8_t Arm::extend(void) {
    logger_debug("[arm.%s]\n", __func__);
    return drive(MODE_FORWARD);
}

uint8_t Arm::force_extend(uint16_t msec, uint32_t mv) {
    return drive(MODE_FORWARD, msec, mv);
}

uint8_t Arm::fold(void) {
    return drive(MODE_REVERSE);
}

uint8_t Arm::force_fold(uint16_t msec, uint32_t mv) {
    return drive(MODE_REVERSE, msec, mv);
}

uint8_t Arm::drive(uint8_t mode) {
    if (mode != MODE_FORWARD && mode != MODE_REVERSE) {
        return RET_ERROR;
    }

    clear_fault();

    uint16_t pos = position_avg();

    if (is_position_abnormal(pos)) {
        logger_error("[arm.%s] abnormal position: %u\n", __func__, pos);
        return RET_ERROR | POTENTIOMETER_ERROR;
    }

    if ((mode == MODE_FORWARD && pos < DRIVE_FORWARD_LIMIT) ||
        (mode == MODE_REVERSE && DRIVE_REVERSE_LIMIT < pos)) {
        logger_error("[arm.%s] Max position\n", __func__);
        return RET_ERROR;
    }

    // Slow start
    for (uint32_t mv = VOUT_MIN_MV; mv < VOUT_LIMIT_MV;
         mv += VOUT_STEP_MV * 4) {
        control(mv, mode);
        wait_us(STEP_INTERVAL_TIME_US);
        Watchdog::get_instance().kick();
    }

    logger_debug("arm.%s: Step up end\n", __func__);

    control(VOUT_LIMIT_MV, mode);

    uint8_t fault = fault_stat();

    timer->stop();
    timer->reset();
    timer->start();

    int msec_start = timer->read_ms();

    while (true) {
        Watchdog::get_instance().kick();
        wait_us(8000);
        pos = position_avg();

        if (is_position_abnormal(pos)) {
            logger_error("[arm.%s] abnormal position: %u\n", __func__, pos);
            fault = fault | POTENTIOMETER_ERROR;
            break;
        }

        if ((mode == MODE_FORWARD && pos < DRIVE_FORWARD_BRAKE_POINT) ||
            (mode == MODE_REVERSE && DRIVE_REVERSE_BRAKE_POINT < pos)) {
            logger_debug("arm.%s: braking at %d\n", __func__, pos);
            break;
        }

        if (timer->read_ms() - msec_start > DRIVE_TIME_MS) {
            timer->stop();
            timer->reset();
            logger_debug("arm.%s: timeout\n", __func__);
            break;
        }
    }

    for (uint32_t mv = VOUT_LIMIT_MV; mv > VOUT_MIN_MV;
         mv -= VOUT_STEP_MV * 4) {
        control(mv, mode);
        wait_us(STEP_INTERVAL_TIME_US);
        pos = position_avg();

        if (is_position_abnormal(pos)) {
            fault |= POTENTIOMETER_ERROR;
            break;
        }

        if ((mode == MODE_FORWARD && pos < DRIVE_FORWARD_LIMIT) ||
            (mode == MODE_REVERSE && DRIVE_REVERSE_LIMIT < pos)) {
            logger_debug("arm.%s Reached limit: %d\n", __func__, pos);
            break;
        }
    }

    stop();

    logger_debug("arm.%s: Step down end\n", __func__);

    if ((fault & (FAULT_I2C | POTENTIOMETER_ERROR | FAULT_OCCURRED)) == 0) {
        return 0;

    } else if ((fault & FAULT_I2C) != 0) {
        return RET_ERROR | RET_ERROR_I2C;

    } else {
        return RET_ERROR | (fault & FAULT_MASK);
    }
}

uint8_t Arm::drive(uint8_t mode, uint16_t msec, uint32_t mv) {
    if (mode != MODE_FORWARD && mode != MODE_REVERSE) {
        return RET_ERROR;
    }

    logger_debug("arm.%s: V = %d\n", __func__, mv);
    msec = 15000 < msec ? 15000 : msec;

    uint32_t max_mv = clamp<uint32_t>(mv, VOUT_MIN_MV, VOUT_MAX_MV);

    // Slow start
    uint32_t step = (max_mv - VOUT_MIN_MV) / 8;
    step = step <= VOUT_STEP_MV ? VOUT_STEP_MV : step;

    clear_fault();

    for (uint32_t mv = VOUT_MIN_MV; mv < max_mv; mv += step) {
        control(mv, mode);
        wait_us(STEP_INTERVAL_TIME_US);
    }

    logger_debug("arm.%s: Step up end\n", __func__);

    control(max_mv, mode);

    uint8_t fault = fault_stat();

    wait_us(msec * 1000);

    for (uint32_t mv = max_mv; mv > VOUT_MIN_MV; mv -= step) {
        control(mv, mode);
        wait_us(STEP_INTERVAL_TIME_US);
    }

    stop();

    logger_debug("arm.%s: Step down end\n", __func__);

    if ((fault & (FAULT_I2C | FAULT_OCCURRED)) == 0) {
        return 0;
    } else if (fault & FAULT_I2C) {
        return RET_ERROR | RET_ERROR_I2C;
    } else {
        return RET_ERROR | (fault & FAULT_MASK);
    }
}

uint16_t Arm::position_avg(void) {
    uint32_t pos = 0;
    for (int16_t t = 0; t < 8; t++) {
        pos += position();
        wait_us(1000);
    }
    pos = pos / 8;

    logger_debug("arm.%s: Position = %d\n", __func__, pos);
    return (uint16_t)pos;
}

uint16_t Arm::position(void) {
    return posmeter->read_u16();
}

bool Arm::is_position_abnormal(uint16_t pos) {
    if (pos < DRIVE_REVERSE_ERROR_GUARD || DRIVE_FORWARD_ERROR_GUARD < pos) {
        logger_error("%s: Abnormal position: %d\n", __func__, pos);
        return true;
    }

    return false;
}
