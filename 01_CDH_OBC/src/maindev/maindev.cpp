/**
 * @brief
 */

#include "maindev.hpp"
#include "common/common.hpp"

MainDevice::MainDevice() {
    i2c = new I2C(PIN_I2C_SDA, PIN_I2C_SCL);
    serial[0] = new RawSerial(PIN_COMM_TX, PIN_COMM_RX, BAUD_COMM);
    serial[1] = new RawSerial(PIN_RPI_TX, PIN_RPI_RX, BAUD_RPI);

    comm = new Commdev(serial[0], PIN_COMM_INT);
    rpi = new RPi(serial[1]);

    tsens = new Tsens(PIN_TEMP, VREF);
    attsens = new BMX055(i2c);
    powsens = new INA219(i2c);

    arm = new Arm(i2c, PINSTAT_LOW, PINSTAT_HIZ, PIN_ARM);
    rw = new DRV8830(i2c, PINSTAT_HIZ, PINSTAT_HIZ);

    led = new LED(LED1);

    power_rpi = new PinCtrl(PIN_POW_RPI);
    rpi->register_pm(power_rpi);
}

MainDevice::~MainDevice() {
    delete power_rpi;
    delete led;
    delete rw;
    delete arm;
    delete powsens;
    delete attsens;
    delete tsens;
    delete comm;
    delete rpi;

    delete serial[0];
    delete serial[1];
    delete i2c;
}
