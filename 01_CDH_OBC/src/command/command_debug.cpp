/**
 * @file
 * @brief
 */

#include "common/logger.hpp"
#include "command.hpp"

#include "command_test.hpp"

void Command::cmd_debug(Telecommand &cmd) {
    uint8_t mode = cmd.data->buf[0];
    logger_info("[%s] mode = %d\n", __func__, mode);

    send_ack(cmd, RESP_OK);

    uint8_t resp = RESP_OK;

    switch (mode) {
        case 0:
            logger_info("%s: BMX055\n", __func__);
            bmx055_sensor_data_t acc, gyr, mag;

            for (int i = 0; i < 200; i++) {
                wait_us(1000 * 100);
                dev->attsens->get_acc(acc);
                dev->attsens->get_gyr(gyr);
                dev->attsens->get_mag(mag);
                logger_base(
                    "%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\n",
                    acc.x, acc.y, acc.z, gyr.x, gyr.y, gyr.z, mag.x, mag.y,
                    mag.z);
            }

            break;

        case 1:
            dev->arm->force_extend(3000, 3050);
            break;

        case 2:
            dev->rw->clear_fault();
            dev->rw->control(3000, DRV8830::MODE_FORWARD);
            wait_us(3 * 1000 * 1000);
            dev->rw->stop();

            break;

        case 11:
            dev->arm->force_fold(3000, 2570);
            break;

        case 12:
            dev->arm->extend();
            break;

        case 13:
            dev->arm->fold();
            break;

        case 21:
            dev->rw->clear_fault();
            dev->rw->control(3000, DRV8830::MODE_REVERSE);
            wait_us(1 * 1000 * 1000);
            dev->rw->stop();

            break;

        case 50:
            for (int i = 0; i < 200; i++) {
                wait_us(1000 * 100);
                logger_info("d\n", dev->arm->position());
            }

            break;

        default:
            logger_error("[%s] Invalid target\n", __func__);
    }
}
