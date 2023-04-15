/**
 * @file
 */

#include "device.hpp"

Device::Device() {
    name = nullptr;
    powdev = nullptr;
    powtimer = nullptr;
    powstat = POWSTAT_NONE;
}

Device::~Device() {
    if (powtimer != nullptr) {
        delete powtimer;
    }
}

int Device::init(void) {
    return 0;
}

int Device::wakeup(void) {
    return 0;
}

int Device::shutdown(void) {
    return 0;
}

void Device::monitor(void) {
}

void Device::register_pm(Device *powdev) {
    this->powdev = powdev;
    powstat = POWSTAT_OFF;

    powtimer = new Timer;
}

int Device::supply_on(int v) {
    return 0;
}

int Device::supply_off(int v) {
    return 0;
}
