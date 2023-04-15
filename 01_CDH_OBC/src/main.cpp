#include "system/system.hpp"
#include "command/command_param.hpp"
#include "net/net.hpp"
#include "terminal/terminal.hpp"

int main(void) {
    logger_config(VERBOSITY);
    Watchdog::get_instance().start(WATCHDOG_TIMEOUT_MS);

    MainDevice * dev = new MainDevice();
    Power *      power = new Power(dev);
    SpaceSocket *sock = new SpaceSocket(dev->comm);
    HKdata *     hk = new HKdata(dev, power, HKDATA_SIZE);
    Command *    cmd = new Command(dev, hk, power, sock);

    MainSystem sys = {dev, power, hk, cmd, sock};

    HouseKeepingTask *hk_task = new HouseKeepingTask(&sys, 20 * 1000);
    BeaconTask *      beacon_task = new BeaconTask(&sys, 120 * 1000);
    LEDTask *         led_task = new LEDTask(&sys, 1000);
    MainTask *        main_task = new MainTask(&sys, 1000);

    SystemTask *tasks[] = {hk_task, beacon_task, led_task, main_task};

    Serial   *pc = new Serial(USBTX, USBRX);
    Terminal *term = new Terminal(pc, &sys);

    while (true) {
        sys.power->run();

        for (size_t i = 0; i < ARRAY_SIZE(tasks); i++) {
            tasks[i]->run();
        }

        term->run();
    }
}
