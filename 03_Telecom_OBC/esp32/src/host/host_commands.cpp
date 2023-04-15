#include "host.hpp"

#include <string.h>

void Host::process_command(comm_op_t &op_in, comm_op_t &op_out) {
    switch (op_in.command) {
        case COMM_CMD_GET_GND_DATA:
            cmd_get_gnd_data(op_in, op_out);
            break;

        case COMM_CMD_DOWNLINK_BEACON:
            cmd_beacon(op_in, op_out);
            break;

        case COMM_CMD_DOWNLINK_DATA:
            cmd_downlink(op_in, op_out);
            break;

        case COMM_CMD_GET_STATUS:
            cmd_get_status(op_in, op_out);
            break;

        default:
            logger_error("%s: command not found: 0x%X\n", __func__,
                         op_in.command);
            break;
    }
}

void Host::cmd_get_status(comm_op_t &op_in, comm_op_t &op_out) {
    int temp = static_cast<int>(temperatureRead());
    logger_debug("%s: Die temp = %d\n", __func__, temp);

    uint8_t *buf = op_out.data.buf;
    u16_to_bytes(queue->next_size(), &buf[0], &buf[1]);
    u32_to_bytes(temp, &buf[2], &buf[3], &buf[4], &buf[5]);

    op_out.data.size = 7;
    op_out.command = op_in.command;
}

void Host::cmd_get_gnd_data(comm_op_t &op_in, comm_op_t &op_out) {
    logger_debug("%s\n", __func__);
    queue->pop(op_out);
    op_out.command = op_in.command;
}

void Host::cmd_downlink(comm_op_t &op_in, comm_op_t &op_out) {
    logger_debug("%s\n", __func__);

    bridge->down.lock->lock();

    memcpy(bridge->down.buf, op_in.data.buf, op_in.data.size);
    bridge->down.size = op_in.data.size;
    bridge->down.prot = RFPROT_BT;

    bridge->down.lock->unlock();

    op_out.data.size = 0;
    op_out.command = op_in.command;
}

void Host::cmd_beacon(comm_op_t &op_in, comm_op_t &op_out) {
    logger_debug("%s\n", __func__);

    bridge->down.lock->lock();

    memcpy(bridge->down.buf, op_in.data.buf, op_in.data.size);
    bridge->down.size = op_in.data.size;
    bridge->down.prot = RFPROT_CW;

    bridge->down.lock->unlock();

    op_out.data.size = 0;
    op_out.command = op_in.command;
}
