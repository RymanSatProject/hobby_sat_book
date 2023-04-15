/**
 * @file
 * @brief
 */

#include "common/common.hpp"
#include "power.hpp"

Power::Power(MainDevice* dev) {
    this->dev = dev;
}

Power::~Power() {
}

void Power::run(void) {
    dev->rpi->monitor();
}
