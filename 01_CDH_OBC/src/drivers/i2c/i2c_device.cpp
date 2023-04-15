#include "i2c_device.hpp"

I2CDevice::I2CDevice() {
    i2caddr = 0;
    online = false;
}

I2CDevice::~I2CDevice() {
}

bool I2CDevice::is_online(void) {
    return online;
}

void I2CDevice::scan(I2C* i2c) {
    char    buf[2] = {0};
    uint8_t num = 0;

    logger_info("I2C devices found:\n");

    for (int a = 3; a < 0x78; a++) {
        i2c->write(a << 1, buf, 1);

        if (i2c->read(a << 1, buf, 2) == 0) {
            logger_info("- 0x%02x\n", a);
            num++;
        }
    }

    if (num == 0) {
        logger_info("none\n");
    }

    logger_info("\n");
}
