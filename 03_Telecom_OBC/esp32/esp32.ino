#include "src/host/host.hpp"
#include "src/rf/rf.hpp"
#include "src/common/common.hpp"
#include "freertos/task.h"

#define BRIDGE_DATA_SIZE 256

bridge_t bridge;
uint8_t  bdata_up[BRIDGE_DATA_SIZE];
uint8_t  bdata_down[BRIDGE_DATA_SIZE];

RF*   rf;
Host* host;

void task_host(void* arg) {
    host = new Host(&bridge);
    host->run();
}

void setup(void) {
    Lock* up_lock = new Lock();
    bridge.up.buf = bdata_up;
    bridge.up.lock = up_lock;

    Lock* down_lock = new Lock();
    bridge.down.buf = bdata_down;
    bridge.down.lock = down_lock;

    logger_init(BAUD_LOG);

    rf = new RF(&bridge);
    rf->init();

    xTaskCreatePinnedToCore(task_host, "task_host", 4096, NULL, 1, NULL, 1);
}

void loop(void) {
    rf->tick();
}
