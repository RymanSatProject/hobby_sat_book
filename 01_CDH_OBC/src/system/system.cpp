/**
 * @file
 * @brief
 */

#include "system.hpp"

SystemTask::SystemTask(MainSystem *_sys, int _interval_ms) {
    sys = _sys;
    interval_ms = _interval_ms;
    prev_ms = 0;
}

SystemTask::~SystemTask() {
}

void SystemTask::run(void) {
    if (SystemClock::msec() > prev_ms + interval_ms) {
        process();
        prev_ms = SystemClock::msec();
    }
}

HouseKeepingTask::HouseKeepingTask(MainSystem *_sys, int _interval_ms)
    : SystemTask(_sys, _interval_ms) {
}

void HouseKeepingTask::process(void) {
    sys->hk->update();
}

BeaconTask::BeaconTask(MainSystem *_sys, int _interval_ms)
    : SystemTask(_sys, _interval_ms) {
}

static uint8_t beacon_data[HKdata::unitsize];

void BeaconTask::process(void) {
    sys->hk->serialize(beacon_data, sys->hk->idx);

    packet_t packet_out = Packet::create(
        CMD_BEACON, true, true, 0, ARRAY_SIZE(beacon_data), 0, beacon_data);

    sys->dev->comm->beacon(packet_out);
}

LEDTask::LEDTask(MainSystem *_sys, int _interval_ms)
    : SystemTask(_sys, _interval_ms) {
}

void LEDTask::process(void) {
    sys->dev->led->flip();
}

MainTask::MainTask(MainSystem *_sys, int _interval_ms)
    : SystemTask(_sys, _interval_ms) {
}

void MainTask::process(void) {
    Watchdog::get_instance().kick();

    sys->dev->comm->update_status();

    Telecommand cmd(COMM_BUF_SIZE);

    if (sys->dev->comm->recvnum == 0) {
        return;
    }

    if (sys->sock->receive(cmd)) {
        return;
    }

    sys->cmd->exec(cmd);
}
