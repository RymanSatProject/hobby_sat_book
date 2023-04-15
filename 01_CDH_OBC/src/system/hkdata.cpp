/**
 * @brief
 */

#include "hkdata.hpp"
#include "common/common.hpp"

HKdata::HKdata(MainDevice *dev, Power *power, int size) {
    this->dev = dev;
    this->power = power;
    this->size = size;
    this->data = new hkunit_t[size];
    memset(data, 0, sizeof(hkunit_t) * size);
    latest = &data[0];

    // Make recording start from 0
    idx = size - 1;
}

HKdata::~HKdata() {
    delete[] data;
}

void HKdata::update(void) {
    idx = (idx + 1) % size;
    latest = &data[idx];

    latest->current_ms = SystemClock::msec();
    latest->temp = (int32_t)dev->tsens->read();
    latest->power_mw = (uint16_t)dev->powsens->power_mw();
    latest->current_ma = (uint16_t)dev->powsens->current_ma();
    latest->bus_vol_mv = (uint16_t)dev->powsens->bus_vol_mv();
    latest->powstat = dev->rpi->powstat;

    bmx055_sensor_data_t acc, gyr, mag;

    if (dev->attsens->get_acc(acc) == 0) {
        latest->acc[0] = 1000000 * acc.x;
        latest->acc[1] = 1000000 * acc.y;
        latest->acc[2] = 1000000 * acc.z;
    } else {
        latest->acc[0] = latest->acc[1] = latest->acc[2] = 0;
    }

    if (dev->attsens->get_gyr(gyr) == 0) {
        latest->gyr[0] = 1000000 * gyr.x;
        latest->gyr[1] = 1000000 * gyr.y;
        latest->gyr[2] = 1000000 * gyr.z;
    } else {
        latest->gyr[0] = latest->gyr[1] = latest->gyr[2] = 0;
    }

    if (dev->attsens->get_mag(mag) == 0) {
        latest->mag[0] = 1000 * mag.x;
        latest->mag[1] = 1000 * mag.y;
        latest->mag[2] = 1000 * mag.z;
    } else {
        latest->mag[0] = latest->mag[1] = latest->mag[2] = 0;
    }

    logger_info("HK[%u] --------------------------\n", idx);
    logger_info("  msec = %d, temp = %d\n", latest->current_ms, latest->temp);
    if (dev->powsens->is_online()) {
        logger_info("  mw = %d ma = %d mv = %d\n", latest->power_mw,
                    latest->current_ma, latest->bus_vol_mv);
    }

    logger_info("  powstat = %02x\n", latest->powstat);

    if (dev->attsens->is_online()) {
        logger_info("  acc = [%d, %d, %d]\n", latest->acc[0], latest->acc[1],
                    latest->acc[2]);
        logger_info("  gyr = [%d, %d, %d]\n", latest->gyr[0], latest->gyr[1],
                    latest->gyr[2]);
        logger_info("  mag = [%d, %d, %d]\n", latest->mag[0], latest->mag[1],
                    latest->mag[2]);
    }
    logger_info("---------------------------------\n");
}

/**
 * @brief Store the HK data with the specified ID to an array
 * @param buf
 * @param idx
 */
void HKdata::serialize(uint8_t buf[], int idx) {
    if (idx > size) {
        logger_error("hk.%s: Index too large: %u", __func__, idx);
        return;
    }

    hkunit_t *hk = &data[idx];

    uint16_t pos = 0;

    u16_to_bytes((uint16_t)idx, &buf[0], &buf[1]);
    u32_to_bytes(hk->current_ms, &buf[2], &buf[3], &buf[4], &buf[5]);
    u32_to_bytes(hk->temp, &buf[6], &buf[7], &buf[8], &buf[9]);
    u16_to_bytes(hk->power_mw, &buf[10], &buf[11]);
    u16_to_bytes(hk->current_ma, &buf[12], &buf[13]);
    u16_to_bytes(hk->bus_vol_mv, &buf[14], &buf[15]);
    buf[16] = dev->rpi->powstat;
    u32_to_bytes(hk->acc[0], &buf[17], &buf[18], &buf[19], &buf[20]);
    u32_to_bytes(hk->acc[1], &buf[21], &buf[22], &buf[23], &buf[24]);
    u32_to_bytes(hk->acc[2], &buf[25], &buf[26], &buf[27], &buf[28]);
    u32_to_bytes(hk->gyr[0], &buf[29], &buf[30], &buf[31], &buf[32]);
    u32_to_bytes(hk->gyr[1], &buf[33], &buf[34], &buf[35], &buf[36]);
    u32_to_bytes(hk->gyr[2], &buf[37], &buf[38], &buf[39], &buf[40]);
    u32_to_bytes(hk->mag[0], &buf[41], &buf[42], &buf[43], &buf[44]);
    u32_to_bytes(hk->mag[1], &buf[45], &buf[46], &buf[47], &buf[48]);
    u32_to_bytes(hk->mag[2], &buf[49], &buf[50], &buf[51], &buf[52]);

    logger_debug("hk::serialize: \n");
    logger_hexdump(VERB_DEBUG, buf, 53);
}
